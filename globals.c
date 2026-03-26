#include "simulator.h"

// --- Statistics Structures ---
BatterStat batter_stats[2][12] = {0}; 
BowlerStat bowler_stats[2][6] = {0};  

// --- Global Match State ---
int current_innings = 0; 
int target_score = -1;   
int team_scores[2] = {0};
int team_wickets[2] = {0};
int team_balls[2] = {0};

int global_score = 0;
int total_balls_bowled = 0;
int wickets_fallen = 0;
bool match_over = false;
bool ball_ready = false; 
bool ball_in_air = false;

// --- Extras & Free Hit State Machine ---
bool current_is_wide = false;
bool current_is_no_ball = false;
bool active_free_hit = false;

// --- Strike & Scheduling Management ---
int striker_id = 1;
int non_striker_id = 2;
int next_batsman_id = 3; 
int active_bowler_id = 1;

pthread_t batsmen[12]; 
int bat_ids[12];

// --- Synchronization Primitives ---
pthread_mutex_t pitch_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t next_ball_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ball_bowled_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t ball_in_air_cond = PTHREAD_COND_INITIALIZER;
sem_t crease_sem; 

// --- Deadlock / Run-out Simulation ---
pthread_mutex_t crease_end_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t crease_end_2 = PTHREAD_MUTEX_INITIALIZER;
bool run_out_in_progress = false;