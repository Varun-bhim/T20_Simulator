#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int runs; int balls; bool is_out; bool has_batted; int fours; int sixes;       
} BatterStat;

typedef struct {
    int runs_conceded; int wickets; int balls_bowled; int wides; int no_balls;    
} BowlerStat;

extern BatterStat batter_stats[2][12];
extern BowlerStat bowler_stats[2][6];  

// --- NEW: OS Time Tracking Variables ---
extern int system_ticks; // The Absolute OS Clock
extern int arrival_time[2][12];
extern int start_time[2][12];
extern int wait_time[2][12];
extern int end_time[2][12]; // Tracks when thread exits
extern bool use_sjf_scheduling; 
extern int spawn_order[12];

extern int current_innings;
extern int target_score;   
extern int team_scores[2];
extern int team_wickets[2];
extern int team_balls[2];
extern int innings_total_deliveries[2];

// Toss variables
extern int toss_winner;
extern int toss_choice;
extern int batting_team;

extern int global_score;
extern int total_balls_bowled; // The Cricket Clock (legal balls only)
extern int total_deliveries;   // All deliveries including wides and no-balls
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

// --- Tracking arrays for Gantt charts ---
extern int striker_per_ball[2][361];
extern int non_striker_per_ball[2][361];
extern int bowler_per_ball[2][361];

extern const char *ind_squad[11];
extern const char *pak_squad[11];
extern const char *ind_bowlers[5];
extern const char *pak_bowlers[5];

extern pthread_mutex_t pitch_mutex;
extern pthread_mutex_t score_mutex;
extern pthread_cond_t next_ball_cond;
extern pthread_cond_t ball_bowled_cond; 
extern pthread_cond_t ball_in_air_cond;
extern sem_t crease_sem; 

extern pthread_mutex_t crease_end_1;
extern pthread_mutex_t crease_end_2;
extern bool run_out_in_progress;

void check_target();
int pick_next_batsman();
void perform_toss();
void reset_innings_state();
void play_innings();
void print_team_innings(int inn_idx, const char* bat_team, const char* bowl_team);
void generate_gantt_chart();
void generate_striker_gantt_chart();
void generate_non_striker_gantt_chart();
void generate_bowler_gantt_chart();
void print_wait_time_analysis();

const char* get_bat_name(int innings, int id);
const char* get_bowl_name(int innings, int id);
const char* get_fielder_name(int innings, int id);

void* fielder_action(void* arg);
void* batsman_action(void* arg);
void* bowler_action(void* arg);

#endif
