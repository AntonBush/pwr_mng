#ifndef MODULE_PWR
#define MODULE_PWR

// todo: ports and pins

// Init output register
void Pwr_init(void);

#define PWR__N_DEVICES 8

#define PWR__DEVICE_0 0
#define PWR__DEVICE_1 1
#define PWR__DEVICE_2 2
#define PWR__DEVICE_3 3

#define PWR__DEVICE_4 4
#define PWR__DEVICE_5 5
#define PWR__DEVICE_6 6
#define PWR__DEVICE_7 7

void Pwr_setCurrentDevice(int device);

#endif // MODULE_PWR
