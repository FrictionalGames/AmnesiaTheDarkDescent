/****************************************************************************/
/*                                                                          */
/* Copyright 1996-1997 by SensAble Devices, Incorporated.					*/
/*	All rights reserved.													*/
/*                                                                          */
/*         Author(s):  Chris Tarr, Thomas H. Massie, Josh Handley			*/
/*                                                                          */
/*														Version:  1.0       */
/*                                                                          */
/*              Header file for Basic Phantom I/O Library			        */
/*																			*/
/* Windows NT 3.51, 4.0														*/
/* Irix 6.2																	*/
/*                                                                          */
/****************************************************************************/
//
// NOT FOR RELEASE
//

#define PHTM_LOW_CHANNEL_SELECT 0
#define PHTM_HIGH_CHANNEL_SELECT 1
#define PHTM_RESET      1
#define PHTM_NORESET    0

/* for accessing elements in vectors stored as float[3] */
#define PHTM_X         0
#define PHTM_Y          1
#define PHTM_Z          2

/* Error Codes */
#define PHTM_WARM_MOTORS -1
#define PHTM_MAX_FORCE -2
#define PHTM_BAD_CHANNEL_SELECT -3
#define PHTM_DEV_FAULT -4
#define PHTM_BAD_FILE -5
#define PHTM_BAD_FILE_NAME -6
#define PHTM_MEMORY_ERROR -8
#define PHTM_DRIVER_ERROR -10
#define PHTM_BAD_ID -11
#define PHTM_FORCES_DISABLED -12
#define PHTM_EXCEEDED_MAX_VELOCITY -13
#define PHTM_ILLEGAL_UPDATE_RATE -14
#define PHTM_ILLEGAL_MAX_VELOCITY -15
#define PHTM_NO_RESET -16
#define PHTM_SAFETY_DRIVER_ERROR -17
#ifdef __cplusplus
extern "C" {
#endif

/* list of public functions  */
int init_phantom(char *config_file_name);
int phantom_reset(int phantom_id);
int enable_phantom_forces(int phantom_id);
int disable_phantom_forces(int phantom_id);
int disable_phantom_forces_no_wait(int phantom_id);
int phantom_check_for_gimbal(int phantom_id);

int update_phantom(int phantom_id);
int phantom_status(int phantom_id);

int set_phantom_coordinate_system(int phantom_id, float origin[], 
		float x_axis[], float y_axis[], float z_axis[]);
int set_phantom_max_velocity(int phantom_id, float max_vel);
int set_phantom_rate(int phantom_id, float update_rate);

int zero_force_saturation(int phantom_id);
int truncate_force_saturation(int phantom_id);
int send_phantom_force(int phantom_id, float force[]);
int command_motor_voltages(int phantom_id, float force[], float torque[]);

int get_phantom_pos(int phantom_id, float pos[]);
int get_phantom_vel(int phantom_id, float vel[]);
int get_phantom_inst_vel(int phantom_id, float vel[]);
float get_phantom_inst_rate(int phantom_id);
float get_phantom_rate(int phantom_id);
int	get_stylus_switch(int phantom_id);
int get_phantom_temp(int phantom_id, float temperature[]);
int get_stylus_matrix(int phantom_id, float T[4][4]);
int get_phantom_encoders(int phantom_id, float theta[]);
int get_gimbal_encoders(int phantom_id, float theta[]);
float get_phantom_max_stiffness(int phantom_id);

int disable_phantom(int phantom_id);

/* friction routines.  Unsupported for now. No info on these yet. */
int get_phantom_stuck_pos(int phantom_id, float stuck_position[]);
int set_phantom_stuck_pos(int phantom_id, float stuck_position[]);
int calc_phantom_friction(int phantom_id,float friction_force[], float force[],
						  float stiffness, float threshold, float stuck_slip);

char *get_library_version();

#ifdef __cplusplus
}
#endif


/* convenience routines - basic vector operations */
#define add_vectors(force1, force2) { \
	force1[0] += force2[0]; \
	force1[1] += force2[1]; \
	force1[2] += force2[2]; \
}

#define subtract_vectors(force1, force2) { \
	force1[0] -= force2[0]; \
	force1[1] -= force2[1]; \
	force1[2] -= force2[2]; \
}

#define zero_vector(force) { \
	force[0] = 0.0; \
	force[1] = 0.0; \
	force[2] = 0.0; \
}

#define mag_vector(v) (sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))
#define normalize_vector(v1,v2) {v1[0] = v2[0]/mag_vector(v2);   \
			      v1[1] = v2[1]/mag_vector(v2); \
			      v1[2] = v2[2]/mag_vector(v2);}

#define scale_vector(v1,v2,k) {v1[0]=v2[0]*k;v1[1]=v2[1]*k;v1[2]=v2[2]*k;}
#define dot_vectors(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define cross_vectors(v1, v2, res) {\
								res[0] = v1[1]*v2[2] - v2[1]*v1[2]; \
								res[1] = - v1[0]*v2[2] + v2[0]*v1[2]; \
                        res[2] = v1[0]*v2[1] - v2[0]*v1[1];}

#define sqr(x) (x*x)
