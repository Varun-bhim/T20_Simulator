#include "simulator.h"

BatterStat batter_stats[2][12] = {0}; 
BowlerStat bowler_stats[2][6] = {0};  

int system_ticks = 0; // Initialize the Absolute OS Clock
int arrival_time[2][12] = {0};
int start_time[2][12] = {0};
int end_time[2][12] = {0};
int wait_time[2][12] = {0};
bool use_sjf_scheduling = false; 
int spawn_order[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; 

const char *ind_squad[11] = {"G. Gambhir", "Y. Pathan", "R. Uthappa", "Yuvraj Singh", "MS Dhoni", "R. Sharma", "Irfan Pathan", "Harbhajan", "Joginder S.", "Sreesanth", "RP Singh"};
const char *pak_squad[11] = {"M. Hafeez", "I. Nazir", "K. Akmal", "Younis Khan", "S. Malik", "Misbah-ul-Haq", "S. Afridi", "Y. Arafat", "S. Tanvir", "Umar Gul", "M. Asif"};

const char *ind_bowlers[5] = {"RP Singh", "Sreesanth", "Irfan Pathan", "Harbhajan", "Joginder S."};
const char *pak_bowlers[5] = {"M. Asif", "S. Tanvir", "Umar Gul", "S. Afridi", "Y. Arafat"};

int innings_index = 0; 
int batting_team_per_innings[2] = {0}; 
int target_score = -1;   
int team_scores[2] = {0};
int team_wickets[2] = {0};
int team_balls[2] = {0};
int innings_total_deliveries[2] = {0};  // Track total deliveries per innings

// Toss variables
int toss_winner = -1;  // 0 = India, 1 = Pakistan
int toss_choice = -1;  // 0 = Bat, 1 = Bowl
int batting_team = 0;  // 0 = India bats first, 1 = Pakistan bats first
bool is_second_innings = false;  // Track if we're in the chasing (second) innings
int global_score = 0;
int total_balls_bowled = 0;       // Only legal balls
int total_deliveries = 0;         // All deliveries (legal + wides + no-balls)
int wickets_fallen = 0;
bool match_over = false;
bool ball_ready = false; 
bool ball_in_air = false;

bool current_is_wide = false;
bool current_is_no_ball = false;
bool active_free_hit = false;

int striker_id = 1;
int non_striker_id = 2;
int next_batsman_id = 3; 
int active_bowler_id = 1;

pthread_t batsmen[12]; 
int bat_ids[12];

// --- Tracking arrays for Gantt charts ---
int striker_per_ball[2][361] = {0};      // Which batsman is at striker end per delivery
int non_striker_per_ball[2][361] = {0};  // Which batsman is at non-striker end per delivery
int bowler_per_ball[2][361] = {0};       // Which bowler bowls each delivery

pthread_mutex_t pitch_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t next_ball_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ball_bowled_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t ball_in_air_cond = PTHREAD_COND_INITIALIZER;
sem_t crease_sem; 

pthread_mutex_t crease_end_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t crease_end_2 = PTHREAD_MUTEX_INITIALIZER;
bool run_out_in_progress = false;

const char* get_bat_name(int innings, int id) {
    return (batting_team_per_innings[innings] == 0) ? ind_squad[id-1] : pak_squad[id-1];
}
const char* get_bowl_name(int innings, int id) {
    return (batting_team_per_innings[innings] == 0) ? pak_bowlers[id-1] : ind_bowlers[id-1];
}
const char* get_fielder_name(int innings, int id) {
    return (batting_team_per_innings[innings] == 0) ? pak_squad[id-1] : ind_squad[id-1];
}
