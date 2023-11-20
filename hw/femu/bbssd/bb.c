#include "../nvme.h"
#include "./ld-tpftl.h"


static void bb_init_ctrl_str(FemuCtrl *n)
{
    static int fsid_vbb = 0;
    const char *vbbssd_mn = "FEMU BlackBox-SSD Controller";
    const char *vbbssd_sn = "vSSD";

    nvme_set_ctrl_name(n, vbbssd_mn, vbbssd_sn, &fsid_vbb);
}

/* bb <=> black-box */
static void bb_init(FemuCtrl *n, Error **errp)
{
    struct ssd *ssd = n->ssd = g_malloc0(sizeof(struct ssd));

    bb_init_ctrl_str(n);

    ssd->dataplane_started_ptr = &n->dataplane_started;
    ssd->ssdname = (char *)n->devname;
    femu_debug("Starting FEMU in Blackbox-SSD mode ...\n");
    ssd_init(n);
}

static void reset_stat(struct ssd *ssd)
{
    // struct statistics *st = &ssd->stat;

    /*FTL*/
    // st->read_joule = 0;
    // st->write_joule = 0;
    // st->erase_joule = 0;
    // st->joule = 0;

    /*TPFTL*/
    // st->cmt_hit_cnt = 0;
    // st->cmt_miss_cnt = 0;
    // st->cmt_hit_ratio = 0;
    // st->access_cnt = 0;
    // st->read_joule = 0;
    // st->write_joule = 0;
    // st->erase_joule = 0;
    // st->joule = 0;
    
    /*LeaFTL*/
    // st->cmt_hit_cnt = 0;
    // st->cmt_miss_cnt = 0;
    // st->cmt_hit_ratio = 0;
    // st->access_cnt = 0;
    // st->model_hit = 0;
    // st->write_cnt = 0;
    // st->wa_cnt = 0;
    // st->gc_cnt = 0;
    // st->read_joule = 0;
    // st->write_joule = 0;
    // st->erase_joule = 0;
    // st->joule = 0;
    // count_segments(ssd);

    /*LearnedFTL*/
    // st->cmt_hit_cnt = 0;
    // st->cmt_miss_cnt = 0;
    // st->cmt_hit_ratio = 0;
    // st->access_cnt = 0;
    // st->model_hit_num = 0;
    // st->model_use_num = 0;
    // st->read_joule = 0;
    // st->write_joule = 0;
    // st->erase_joule = 0;
    // st->joule = 0;
}

static void print_stat(struct ssd *ssd)
{
    // struct statistics *st = &ssd->stat;
    
    /*ftl*/
    // st->joule = st->read_joule + st->write_joule + st->erase_joule;
    // printf("read joule: %Lf\n", st->read_joule);
    // printf("write joule: %Lf\n", st->write_joule);
    // printf("erase joule: %Lf\n", st->erase_joule);
    // printf("All joule: %Lf\n", st->joule);

    /*tpftl*/
    // if (st->access_cnt == 0) {
    //     st->cmt_hit_ratio = 0;
    // } else {
    //     st->cmt_hit_ratio = (double)st->cmt_hit_cnt / st->access_cnt;
    // }
    // st->joule = st->read_joule + st->write_joule + st->erase_joule;

    // printf("CMT hit count: %lu\n", st->cmt_hit_cnt);
    // printf("CMT miss count: %lu\n", st->cmt_miss_cnt);
    // printf("CMT access count: %lu\n", st->access_cnt);
    // printf("CMT hit ratio: %lf\n", st->cmt_hit_ratio);
    // printf("read joule: %Lf\n", st->read_joule);
    // printf("write joule: %Lf\n", st->write_joule);
    // printf("erase joule: %Lf\n", st->erase_joule);
    // printf("All joule: %Lf\n", st->joule);

    /*LeaFTL*/
    // st->joule = st->read_joule + st->write_joule + st->erase_joule;
    // printf("total cnt: %lld\n", (long long)ssd->stat.access_cnt);
    // printf("cmt cnt: %lld\n", (long long)ssd->stat.cmt_hit_cnt);
    // printf("model cnt: %lld\n", (long long)ssd->stat.model_hit);
    // printf("read joule: %Lf\n", st->read_joule);
    // printf("write joule: %Lf\n", st->write_joule);
    // printf("erase joule: %Lf\n", st->erase_joule);
    // printf("All joule: %Lf\n", st->joule);
    // count_segments(ssd);

    /*LearnedFTL*/
    // st->joule = st->read_joule + st->write_joule + st->erase_joule;
    // printf("total cnt: %lld\n", (long long)ssd->stat.access_cnt);
    // printf("cmt cnt: %lld\n", (long long)ssd->stat.cmt_hit_cnt);
    // printf("model cnt: %lld\n", (long long)ssd->stat.model_hit_num);
    // printf("read joule: %Lf\n", st->read_joule);
    // printf("write joule: %Lf\n", st->write_joule);
    // printf("erase joule: %Lf\n", st->erase_joule);
    // printf("All joule: %Lf\n", st->joule);
}

static void bb_flip(FemuCtrl *n, NvmeCmd *cmd)
{
    struct ssd *ssd = n->ssd;
    int64_t cdw10 = le64_to_cpu(cmd->cdw10);

    switch (cdw10) {
    case FEMU_ENABLE_GC_DELAY:
        ssd->sp.enable_gc_delay = true;
        femu_log("%s,FEMU GC Delay Emulation [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_GC_DELAY:
        ssd->sp.enable_gc_delay = false;
        femu_log("%s,FEMU GC Delay Emulation [Disabled]!\n", n->devname);
        break;
    case FEMU_ENABLE_DELAY_EMU:
        ssd->sp.pg_rd_lat = NAND_READ_LATENCY;
        ssd->sp.pg_wr_lat = NAND_PROG_LATENCY;
        ssd->sp.blk_er_lat = NAND_ERASE_LATENCY;
        ssd->sp.ch_xfer_lat = 0;
        femu_log("%s,FEMU Delay Emulation [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_DELAY_EMU:
        ssd->sp.pg_rd_lat = 0;
        ssd->sp.pg_wr_lat = 0;
        ssd->sp.blk_er_lat = 0;
        ssd->sp.ch_xfer_lat = 0;
        femu_log("%s,FEMU Delay Emulation [Disabled]!\n", n->devname);
        break;
    case FEMU_RESET_ACCT:
        n->nr_tt_ios = 0;
        n->nr_tt_late_ios = 0;
        femu_log("%s,Reset tt_late_ios/tt_ios,%lu/%lu\n", n->devname,
                n->nr_tt_late_ios, n->nr_tt_ios);
        break;
    case FEMU_ENABLE_LOG:
        n->print_log = true;
        femu_log("%s,Log print [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_LOG:
        n->print_log = false;
        femu_log("%s,Log print [Disabled]!\n", n->devname);
        break;
    case FEMU_RESET_STAT:
        reset_stat(ssd);
        femu_log("%s,Reset statistics!\n", n->devname);
        break;
    case FEMU_PRINT_STAT:
        print_stat(ssd);
        femu_log("%s,Statistics print!\n", n->devname);
        break;
    default:
        printf("FEMU:%s,Not implemented flip cmd (%lu)\n", n->devname, cdw10);
    }
}

static uint16_t bb_nvme_rw(FemuCtrl *n, NvmeNamespace *ns, NvmeCmd *cmd,
                           NvmeRequest *req)
{
    return nvme_rw(n, ns, cmd, req);
}

static uint16_t bb_io_cmd(FemuCtrl *n, NvmeNamespace *ns, NvmeCmd *cmd,
                          NvmeRequest *req)
{
    switch (cmd->opcode) {
    case NVME_CMD_READ:
    case NVME_CMD_WRITE:
        return bb_nvme_rw(n, ns, cmd, req);
    default:
        return NVME_INVALID_OPCODE | NVME_DNR;
    }
}

static uint16_t bb_admin_cmd(FemuCtrl *n, NvmeCmd *cmd)
{
    switch (cmd->opcode) {
    case NVME_ADM_CMD_FEMU_FLIP:
        bb_flip(n, cmd);
        return NVME_SUCCESS;
    default:
        return NVME_INVALID_OPCODE | NVME_DNR;
    }
}

int nvme_register_bbssd(FemuCtrl *n)
{
    n->ext_ops = (FemuExtCtrlOps) {
        .state            = NULL,
        .init             = bb_init,
        .exit             = NULL,
        .rw_check_req     = NULL,
        .admin_cmd        = bb_admin_cmd,
        .io_cmd           = bb_io_cmd,
        .get_log          = NULL,
    };

    return 0;
}

