#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// --- Statistics Structures ---
typedef struct {
    int runs;
    int balls;
    bool is_out;
    bool has_batted;
    int fours;       
    int sixes;       
} BatterStat;

typedef struct {
    int runs_conceded;
    int wickets;
    int balls_bowled;
    int wides;       
    int no_balls;    
} BowlerStat;

// --- Global Variables (Extern Declarations) ---
extern BatterStat batter_stats[2][12];
extern BowlerStat bowler_stats[2][6];  

extern int current_innings;
extern int target_score;   
extern int team_scores[2];
extern int team_wickets[2];
extern int team_balls[2];

extern int global_score;
extern int total_balls_bowled;
extern int wickets_fallen;
extern bool match_over;
extern bool ball_ready; 
extern bool ball_in_air;

extern bool current_is_wide;
extern bool current_is_no_ball;
extern bool active_free_hit;

extern int striker_id;
extern int non_striker_id;
extern int next_batsman_id; 
extern int active_bowler_id;

extern pthread_t batsmen[12]; 
extern int bat_ids[12];

// --- Synchronization Primitives (Extern Declarations) ---
extern pthread_mutex_t pitch_mutex;
extern pthread_mutex_t score_mutex;
extern pthread_cond_t next_ball_cond;
extern pthread_cond_t ball_bowled_cond; 
extern pthread_cond_t ball_in_air_cond;
extern sem_t crease_sem; 

extern pthread_mutex_t crease_end_1;
extern pthread_mutex_t crease_end_2;
extern bool run_out_in_progress;

// --- Function Prototypes ---
void check_target();
void reset_innings_state();
void play_innings();
void print_team_innings(int inn_idx, const char* bat_team, const char* bowl_team);

void* fielder_action(void* arg);
void* batsman_action(void* arg);
void* bowler_action(void* arg);

#endif // SIMULATOR_H