#include <linux/virtio.h>

uint8_t virtqueue[];
struct virtq * vq = virtqueue;
struct virtio_blk_config * disk_cfg;


uint8_t virt_buffer[];
uint8_t volatile virt_status;
struct virtio_blk_req virt_request = {0, 0, 0};





void virtio_mmio_init(void) {
    if(*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
     *R(VIRTIO_MMIO_VERSION) != 1 ||
     *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
     *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
        panic("Could not find virtio disk");
    }

    //  根据3.1.1 Driver Requirements: Device Initialization来处理
    //  由于Legacy Interface, 根据3.1.2 Legacy Interface: Device Initialization:
    //  The result was the steps 5 and 6 were omitted, and steps 4, 7 and 8 were conflated.

    uint32_t status = 0x0;
    //  写0 reset, 非0设置对应位

    //  1. Reset the device.
    *R(VIRTIO_MMIO_STATUS) = status;
    //  2. Set the ACKNOWLEDGE status bit: the guest OS has noticed the device
    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    *R(VIRTIO_MMIO_STATUS) = status;
    //  3. Set the DRIVER status bit: the guest OS knows how to drive the device.
    status |= VIRTIO_CONFIG_S_DRIVER;
    *R(VIRTIO_MMIO_STATUS) = status;
    //  4.  Read device feature bits ... driver MAY read (but MUST NOT write) the device-specific configuration ...
    //  搞不懂为什么xv6-riscv这里用了uint64

    uint32_t features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);            //  解除只读
    features &= ~(1 << VIRTIO_BLK_F_SCSI);          //  不知道为什么要设, 反正xv6清除了
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);    //  是在设备的缓存中, 由设备决定何时写回还是直接透写不缓存, 设0透写 (缓存时可以用flush指令强制写回)
    features &= ~(1 << VIRTIO_BLK_F_MQ);            //  N=1 if VIRTIO_BLK_F_MQ is not negotiated, otherwise N is set by num_queues.
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);        //  这是一个与Legacy Interface有关的feature, 与向device传递信息的布局有关
                                                    //  如果在Legacy Interface不支持支持这个位, 那么必须遵照每一设备的Legacy Interface: Framing Requirements
                                                    //  中的指示来操作。例如, 不支持这个位就意味着传递一次blk指令必须使用3个desc。
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);    //  个人理解是中断的触发条件, 如果不设置, 由flag决定; 
                                                    //  如果设置了, 将由avail ring和used ring的各自的idx和avail event和used event决定
                                                    //  xv6在此处则是自己维持了一个used_idx在中断发生时处理这一切(好像涉及到第一次中断发生后使用轮训等待完成)
                                                    //  这个位在我现在使用的qemu 7.0.0中是支持使用的, 我可能考虑使用这个位的功能。
                                                    //  另外, 在使用avail中的idx时, 作为driver的我们只能让它增加。即使在使用的时候用的是 idx % QUEUE_SIZE
                                                    //  我们不行将idx % QUEUE_SIZE写回到idx中, 这样做等于手工在减小。
                                                    //  溢出问题在2.7.13.3 Updating idx说明
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);//  设置了可以间接引用描述符表
    *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;
    //  5、6 omitted
    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    *R(VIRTIO_MMIO_STATUS) = status;

    status |= VIRTIO_CONFIG_S_DRIVER_OK;
    *R(VIRTIO_MMIO_STATUS) = status;

    *R(VIRTIO_MMIO_GUEST_PAGE_SIZE) = PGSIZE;
    
    //  QUEUE_SEL和QueueNotify等其实设置的是index of queues
    //  就是说有多个queue
    //  QueueNum实际上说的是每个Queue的queue size
    // initialize queue 0.
    *R(VIRTIO_MMIO_QUEUE_SEL) = 0;
    uint32_t max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
    if(max == 0)
        panic("virtio disk has no queue 0");
    if(max < QUEUE_SIZE)
        panic("virtio disk max queue too short");
    *R(VIRTIO_MMIO_QUEUE_NUM) = QUEUE_SIZE;
    for (int i = 0; i < PGSIZE * 2; i++) {
        virtqueue[i] = 0x0;
    }
    *R(VIRTIO_MMIO_QUEUE_PFN) = ((ssize_t)virtqueue) >> PGSHIFT;

    disk_cfg = (void *)R(VIRTIO_MMIO_CONFIG);

    // printk("cap: %x cyl: %d, head: %d sec: %d\n", disk_cfg->capacity, disk_cfg->geometry.cylinders, disk_cfg->geometry.heads, disk_cfg->geometry.sectors);
}


void virtio_cmd(int write, uint64_t sector) {
    if(write)
        virt_request.type = VIRTIO_BLK_T_OUT; // write the disk
    else
        virt_request.type = VIRTIO_BLK_T_IN; // read the disk
    virt_request.reserved = 0;
    virt_request.sector = sector;

    vq->desc[0].addr = (uint64_t)&virt_request;
    vq->desc[0].len  = sizeof(struct virtio_blk_req);
    vq->desc[0].flags= VIRTQ_DESC_F_NEXT;
    vq->desc[0].next = 1;

    vq->desc[1].addr = (uint64_t)virt_buffer;
    vq->desc[1].len  = 512; //无论config里怎么设置, 这个都是512
    if(write)
        vq->desc[1].flags= 0; 
    else
        vq->desc[1].flags = VIRTQ_DESC_F_WRITE;
    vq->desc[1].flags |= VIRTQ_DESC_F_NEXT;
    vq->desc[1].next = 2;

    virt_status = 0xFF; //  填1以供device写后证实确实写0了
    vq->desc[2].addr = (uint64_t)&virt_status;
    vq->desc[2].len  = 1;
    vq->desc[2].flags= VIRTQ_DESC_F_WRITE;
    vq->desc[2].next = 0;

    //  就像之前说的, 具体用的时候要mod, 但是维护的时候只能单增
    vq->avail.ring[vq->avail.idx % QUEUE_SIZE] = 0;
    __sync_synchronize();
    //  Once available idx is updated by the driver, this exposes the descriptor and its contents. The device MAY access the descriptor chains the driver created and the memory they refer to immediately.
    vq->avail.idx += 1;
    __sync_synchronize();
    *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0;   //  queue index, 我们只有一个queue
    // Notifications分类见2.3 Notifications
    //  在没有VIRTIO_F_NOTIFICATION_DATA时, 通过2.9 Driver Notifications中所说发送Notifications
}

void VIRT_ACK(void) {
    *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;
}

__attribute__((__aligned__(PGSIZE)))
uint8_t virtqueue[PGSIZE * 2];

__attribute__((__aligned__(PGSIZE)))
uint8_t virt_buffer[512];
