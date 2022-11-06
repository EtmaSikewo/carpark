#define MEDIAN_WINDOW 5
#define TEMPCHANGE_WINDOW 30

struct tempnode {
	int temperature;
	struct tempnode *next;
};
