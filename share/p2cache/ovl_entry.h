#ifndef OVL_ENTRY
#define OVL_ENTRY

#include <linux/fs.h>

/*
 * The ovl filesystem constants/structures
 */

/*
 * Mount flags
 */
#define PCACHE_MOUNT_PROTECT      0x000001    /* wprotect CR0.WP */
#define PCACHE_MOUNT_XATTR_USER   0x000002    /* Extended user attributes */
#define PCACHE_MOUNT_POSIX_ACL    0x000004    /* POSIX Access Control Lists */
#define PCACHE_MOUNT_DAX          0x000008    /* Direct Access */
#define PCACHE_MOUNT_ERRORS_CONT  0x000010    /* Continue on errors */
#define PCACHE_MOUNT_ERRORS_RO    0x000020    /* Remount fs ro on errors */
#define PCACHE_MOUNT_ERRORS_PANIC 0x000040    /* Panic on errors */
#define PCACHE_MOUNT_HUGEMMAP     0x000080    /* Huge mappings with mmap */
#define PCACHE_MOUNT_HUGEIOREMAP  0x000100    /* Huge mappings with ioremap */
#define PCACHE_MOUNT_FORMAT       0x000200    /* was FS formatted on mount? */
#define PCACHE_MOUNT_DATA_COW     0x000400    /* Copy-on-write for data integrity */


/*
 * Maximal count of links to a file
 */
#define ovl_LINK_MAX          32000

#define ovl_DEF_BLOCK_SIZE_4K 4096

#define ovl_INODE_BITS   7
#define ovl_INODE_SIZE   128    /* must be power of two */

#define ovl_NAME_LEN 255

#define MAX_CPUS 1024

/* ======================= Reserved blocks ========================= */

/*
 * Block 0 contains super blocks;
 * Block 1 contains reserved inodes;
 * Block 2 - 15 are reserved.
 * Block 16 - 31 contain pointers to inode table.
 * Block 32 - 47 contain pointers to replica inode table.
 * Block 48 - 63 contain pointers to journal pages.
 *
 * If data protection is enabled, more blocks are reserverd for checksums and
 * parities and the number is derived according to the whole storage size.
 */
// #define	HEAD_RESERVED_BLOCKS	8388608
// #define HEAD_RESERVED_BLOCKS 25165824UL
#define HEAD_RESERVED_BLOCKS 44040192UL
#define	NUM_JOURNAL_PAGES	16

#define SUPER_BLOCK_START       0 // Superblock
#define	RESERVE_INODE_START	1 // Reserved inodes
#define	INODE_TABLE0_START	16 // inode table
#define	INODE_TABLE1_START	32 // replica inode table
#define	JOURNAL_START		48 // journal pointer table

/* For replica super block and replica reserved inodes */
#define	TAIL_RESERVED_BLOCKS	2

/* ======================= Reserved inodes ========================= */

/* We have space for 31 reserved inodes */
#define ovl_ROOT_INO		(1)
#define ovl_INODETABLE_INO	(2)	/* Fake inode associated with inode
					 * stroage.  We need this because our
					 * allocator requires inode to be
					 * associated with each allocation.
					 * The data actually lives in linked
					 * lists in INODE_TABLE0_START. */
#define ovl_BLOCKNODE_INO	(3)     /* Storage for allocator state */
#define ovl_LITEJOURNAL_INO	(4)     /* Storage for lightweight journals */
#define ovl_INODELIST_INO	(5)     /* Storage for Inode free list */
#define ovl_SNAPSHOT_INO	(6)	/* Storage for snapshot state */
#define ovl_TEST_PERF_INO	(7)


/* Normal inode starts at 32 */
#define ovl_NORMAL_INODE_START      (32)

struct dlist_node {
	struct dlist_node *prev;
	struct dlist_node *next;
	int start;
	int end;
	void* ptr;
};

struct ovl_sb_info {
    	struct super_block *sb;			/* VFS super block */
	struct ovl_super_block *ovl_sb;	/* DRAM copy of SB */
	struct block_device *s_bdev;
	struct dax_device *s_dax_dev;

	/*
	 * base physical and virtual address of ovl (which is also
	 * the pointer to the super block)
	 */
	phys_addr_t	phys_addr;
	void		*virt_addr;
	void		*replica_reserved_inodes_addr;
	void		*replica_sb_addr;

	unsigned long	num_blocks;

	/* TODO: Remove this, since it's unused */
	/*
	 * Backing store option:
	 * 1 = no load, 2 = no store,
	 * else do both
	 */
	unsigned int	ovl_backing_option;

	/* Mount options */
	unsigned long	bpi;
	unsigned long	blocksize;
	unsigned long	initsize;
	unsigned long	s_mount_opt;
	kuid_t		uid;    /* Mount uid for root directory */
	kgid_t		gid;    /* Mount gid for root directory */
	umode_t		mode;   /* Mount mode for root directory */
	atomic_t	next_generation;
	/* inode tracking */
	unsigned long	s_inodes_used_count;
	unsigned long	head_reserved_blocks;
	unsigned long	tail_reserved_blocks;

	struct mutex	s_lock;	/* protects the SB's buffer-head */

	int cpus;
	struct proc_dir_entry *s_proc;

	/* Snapshot related */
	// struct ovl_inode_info	*snapshot_si;
	// struct radix_tree_root	snapshot_info_tree;
	int num_snapshots;
	/* Current epoch. volatile guarantees visibility */
	volatile u64 s_epoch_id;
	volatile int snapshot_taking;

	int mount_snapshot;
	u64 mount_snapshot_epoch_id;

	struct task_struct *snapshot_cleaner_thread;
	wait_queue_head_t snapshot_cleaner_wait;
	wait_queue_head_t snapshot_mmap_wait;
	void *curr_clean_snapshot_info;

	struct task_struct *bg_thread;
	wait_queue_head_t bg_wait;
	volatile atomic_t bg_signal;
	// wait_queue_head_t bg_wait;

	/* DAX-mmap snapshot structures */
	struct mutex vma_mutex;
	struct list_head mmap_sih_list;

	/* ZEROED page for cache page initialized */
	void *zeroed_page;

	/* Checksum and parity for zero block */
	u32 zero_csum[8];
	void *zero_parity;

	/* Per-CPU journal lock */
	spinlock_t *journal_locks;

	/* Per-CPU inode map */
	struct inode_map	*inode_maps;

	/* Decide new inode map id */
	unsigned long map_id;

	/* Per-CPU free block list */
	struct free_list *free_lists;
	unsigned long per_list_blocks;

	struct dlist_node dlist[32][128];

	unsigned long tmp_addr[32][128];

	// struct dlist_node* dlist_head[32];

	int write_back_sync;
	int enable_dram_page;
	int enable_dram_log;

	void* dram_pool;
	unsigned long dram_alloc[128];

	struct file *underlying_file;
	void *bio_buffer;	

};

struct ovl_inode_info_header {
	/* Map from file offsets to write log entries. */
	struct radix_tree_root tree;
	struct rb_root rb_tree;		/* RB tree for directory */
	struct rb_root vma_tree;	/* Write vmas */
	struct list_head list;		/* SB list of mmap sih */
	int num_vmas;
	unsigned short i_mode;		/* Dir or file? */
	unsigned int i_flags;
	unsigned long log_pages;	/* Num of log pages */
	unsigned long i_size;
	unsigned long i_blocks;
	unsigned long ino;
	unsigned long pi_addr;
	unsigned long alter_pi_addr;
	unsigned long valid_entries;	/* For thorough GC */
	unsigned long num_entries;	/* For thorough GC */
	u64 last_setattr;		/* Last setattr entry */
	u64 last_link_change;		/* Last link change entry */
	u64 last_dentry;		/* Last updated dentry */
	u64 trans_id;			/* Transaction ID */
	u64 log_head;			/* Log head pointer */
	u64 log_tail;			/* Log tail pointer */
	u64 alter_log_head;		/* Alternate log head pointer */
	u64 alter_log_tail;		/* Alternate log tail pointer */
	u8  i_blk_type;

	struct xarray dram_pages; 
	struct xarray pmem_pages;

	// struct rb_root log_index;
	struct xarray log_index;
	struct xarray partial_writes;
};

struct ovl_inode_info {
	// union {
	// 	struct ovl_dir_cache *cache;	/* directory */
	// 	struct inode *lowerdata;	/* regular file */
	// };
	// const char *redirect;
	// u64 version;
	// unsigned long flags;
	struct inode vfs_inode;
	struct ovl_inode_info_header header;
	// struct address_space 
	// struct dentry *__upperdentry;
	// struct inode *lower;

	// /* synchronize copy up and more */
	// struct mutex lock;
};

static inline struct ovl_inode_info *OVL_I(struct inode *inode)
{
	return container_of(inode, struct ovl_inode_info, vfs_inode);
}

static inline struct ovl_inode_info_header *OVL_IH(struct inode *inode)
{
	struct ovl_inode_info *si = OVL_I(inode);
	return &si->header;
}

typedef unsigned char  u8;
typedef unsigned long long __le64;

struct ovl_dir_log_entry {
    u8 entry_type; // dir operation type
    u8 name_len; // child name size
    u8 reassigned;
    u8 invalid;
    u8 padding0[4];
	unsigned long p_ino; // parent inode number
	unsigned long c_ino; // child inode number
	u8 padding1[40];
    char	name[255 + 1]; // child name
} __attribute((__packed__));

struct ovl_file_write_entry {
	u8	entry_type;
	u8	write_type;

	u16 padding;
	
	__le64 mtime; // modify time
	__le64 ino; // inode number
	__le64 pos; // file position
	__le64 blocknr; // blocknr to pm
	__le64 len; // len

	struct inode *inode;



	// u8	reassigned;	/* Data is not latest */
	// u8	updating;	/* Data is being written */
	// u8	padding;
	// __le32	num_pages;
	// __le64	block;          /* offset of first block in this write */
	// __le64	pgoff;          /* file offset at the beginning of this write */
	// __le32	invalid_pages;	/* For GC */
	// /* For both ctime and mtime */
	// __le32	mtime;
	// __le64	size;           /* File size after this write */
	// __le64	epoch_id;
	// __le64	trans_id;
	// __le32	csumpadding;
	// __le32	csum;
};

struct journal_ptr_pair {
	__le64 journal_head;
	__le64 journal_tail;
};

static inline struct ovl_sb_info *OVL_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

struct ovl_config {
	// char *lowerdir;
	char *upperdir;
	// char *workdir;
	bool default_permissions;
	bool redirect_dir;
	bool redirect_follow;
	const char *redirect_mode;
	bool index;
	bool nfs_export;
	int xino;
	bool metacopy;
};


struct ovl_fs {
	struct vfsmount *upper_mnt;
	unsigned int numlower;
	/* Number of unique lower sb that differ from upper sb */
	unsigned int numlowerfs;
	// struct ovl_layer *lower_layers;
	struct ovl_sb *lower_fs;
	/* workbasedir is the path at workdir= mount option */
	struct dentry *workbasedir;
	/* workdir is the 'work' directory under workbasedir */
	struct dentry *workdir;
	/* index directory listing overlay inodes by origin file handle */
	struct dentry *indexdir;
	long namelen;
	/* pathnames of lower and upper dirs, for show_options */
	struct ovl_config config;
	/* creds of process who forced instantiation of super block */
	const struct cred *creator_cred;
	bool tmpfile;
	bool noxattr;
	/* Did we take the inuse lock? */
	bool upperdir_locked;
	bool workdir_locked;
	/* Traps in ovl inode cache */
	struct inode *upperdir_trap;
	struct inode *workbasedir_trap;
	struct inode *workdir_trap;
	struct inode *indexdir_trap;
	/* Inode numbers in all layers do not use the high xino_bits */
	unsigned int xino_bits;
};

#define ovl_BLOCK_TYPE_4K     0
#define ovl_BLOCK_TYPE_2M     1
#define ovl_BLOCK_TYPE_1G     2
#define ovl_BLOCK_TYPE_MAX    3

#endif