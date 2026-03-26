#include "simulator.h"

void* fielder_action(void* arg) {
    int fielder_id = *((int*)arg);
    while (!match_over) {
        pthread_mutex_lock(&pitch_mutex);
        while (!ball_in_air && !match_over) {
            pthread_cond_wait(&ball_in_air_cond, &pitch_mutex);
        }
        if (match_over) {
            pthread_mutex_unlock(&pitch_mutex);
            break;
        }
        ball_in_air = false; 
        pthread_mutex_unlock(&pitch_mutex);
        usleep(15000); 
    }
    return NULL;
}

void* batsman_action(void* arg) {
    int bat_id = *((int*)arg);
    
    sem_wait(&crease_sem);
    batter_stats[current_innings][bat_id].has_batted = true;

    while (!match_over && wickets_fallen < 10) {
        pthread_mutex_lock(&pitch_mutex);
        
        while ((!ball_ready || striker_id != bat_id) && !match_over && !run_out_in_progress) {
            pthread_cond_wait(&ball_bowled_cond, &pitch_mutex);
        }
        
        if (match_over) {
            pthread_mutex_unlock(&pitch_mutex);
            break;
        }

        if (run_out_in_progress && bat_id == non_striker_id) {
            pthread_mutex_unlock(&pitch_mutex); 
            pthread_mutex_lock(&crease_end_2); 
            pthread_mutex_lock(&crease_end_1); 
            pthread_mutex_unlock(&crease_end_1);
            pthread_mutex_unlock(&crease_end_2);
            continue; 
        }

        if (bat_id == striker_id && ball_ready) {
            pthread_mutex_lock(&score_mutex);
            
            if (current_is_wide) {
                global_score += 1;
                bowler_stats[current_innings][active_bowler_id].runs_conceded += 1;
                bowler_stats[current_innings][active_bowler_id].wides += 1; 
                printf("[Umpire] WIDE! Score: %d/%d\n", global_score, wickets_fallen);
                check_target();
                pthread_mutex_unlock(&score_mutex);
                ball_ready = false; 
                pthread_cond_signal(&next_ball_cond); 
                pthread_mutex_unlock(&pitch_mutex);
                usleep(5000); 
                continue; 
            } else if (current_is_no_ball) {
                global_score += 1;
                bowler_stats[current_innings][active_bowler_id].runs_conceded += 1;
                bowler_stats[current_innings][active_bowler_id].no_balls += 1; 
                printf("[Umpire] NO BALL! FREE HIT next delivery!\n");
                check_target();
            }

            batter_stats[current_innings][bat_id].balls++;
            if (!current_is_no_ball) bowler_stats[current_innings][active_bowler_id].balls_bowled++;
            pthread_mutex_unlock(&score_mutex);

            int shot_probability = rand() % 100;
            int runs_scored = 0;
            bool is_wicket = false;
            bool is_run_out = false;

            if (bat_id <= 7) {
                if (shot_probability < 30) runs_scored = 0;          
                else if (shot_probability < 60) runs_scored = 1;     
                else if (shot_probability < 70) runs_scored = 2;     
                else if (shot_probability < 85) runs_scored = 4;     
                else if (shot_probability < 94) runs_scored = 6;     
                else if (shot_probability < 96) is_run_out = true;   
                else is_wicket = true;                               
            } else {
                if (shot_probability < 55) runs_scored = 0;          
                else if (shot_probability < 75) runs_scored = 1;     
                else if (shot_probability < 80) runs_scored = 2;     
                else if (shot_probability < 84) runs_scored = 4;     
                else if (shot_probability < 85) runs_scored = 6;     
                else if (shot_probability < 90) is_run_out = true;   
                else is_wicket = true;                               
            }

            if (is_run_out) {
                run_out_in_progress = true;
                pthread_cond_broadcast(&ball_bowled_cond); 
                pthread_mutex_unlock(&pitch_mutex); 

                pthread_mutex_lock(&crease_end_1); 
                usleep(10000); 

                if (pthread_mutex_trylock(&crease_end_2) != 0) {
                    pthread_mutex_lock(&pitch_mutex);
                    pthread_mutex_lock(&score_mutex);
                    
                    printf(">>> [Umpire] CIRCULAR WAIT! Batsman %d RUN OUT! <<<\n", bat_id);
                    wickets_fallen++;
                    batter_stats[current_innings][bat_id].is_out = true;
                    run_out_in_progress = false; 
                    
                    pthread_mutex_unlock(&crease_end_1); 

                    if (wickets_fallen == 10) {
                        match_over = true;
                        pthread_cond_broadcast(&next_ball_cond);
                    } else {
                        int new_bat_id = next_batsman_id++;
                        bat_ids[new_bat_id] = new_bat_id;
                        striker_id = new_bat_id; 
                        pthread_create(&batsmen[new_bat_id], NULL, batsman_action, &bat_ids[new_bat_id]);
                    }

                    active_free_hit = false; 
                    pthread_mutex_unlock(&score_mutex);
                    ball_ready = false; 
                    pthread_cond_signal(&next_ball_cond); 
                    pthread_mutex_unlock(&pitch_mutex);
                    sem_post(&crease_sem); 
                    pthread_exit(NULL); 
                } else {
                    pthread_mutex_unlock(&crease_end_2);
                    pthread_mutex_unlock(&crease_end_1);
                    runs_scored = 1;
                    run_out_in_progress = false;
                    pthread_mutex_lock(&pitch_mutex); 
                }
            }

            if (is_wicket && !is_run_out) {
                if (active_free_hit || current_is_no_ball) {
                    runs_scored = 0; 
                    is_wicket = false; 
                } else {
                    pthread_mutex_lock(&score_mutex);
                    printf(">>> WICKET! Batsman %d is OUT! <<<\n", bat_id);
                    wickets_fallen++;
                    batter_stats[current_innings][bat_id].is_out = true;
                    bowler_stats[current_innings][active_bowler_id].wickets++;

                    if (wickets_fallen == 10) {
                        match_over = true;
                        pthread_cond_broadcast(&next_ball_cond);
                    } else {
                        int new_bat_id = next_batsman_id++;
                        bat_ids[new_bat_id] = new_bat_id;
                        striker_id = new_bat_id; 
                        pthread_create(&batsmen[new_bat_id], NULL, batsman_action, &bat_ids[new_bat_id]);
                    }
                    
                    pthread_mutex_unlock(&score_mutex);
                    ball_ready = false; 
                    pthread_cond_signal(&next_ball_cond); 
                    pthread_mutex_unlock(&pitch_mutex);
                    sem_post(&crease_sem); 
                    pthread_exit(NULL); 
                }
            }

            if (!is_wicket && !is_run_out) {
                pthread_mutex_lock(&score_mutex);
                global_score += runs_scored;
                batter_stats[current_innings][bat_id].runs += runs_scored;
                bowler_stats[current_innings][active_bowler_id].runs_conceded += runs_scored;

                if (runs_scored == 4) batter_stats[current_innings][bat_id].fours++;
                if (runs_scored == 6) batter_stats[current_innings][bat_id].sixes++;

                if (runs_scored == 0) printf("[Bat %d] Dot ball.\n", bat_id);
                else printf("[Bat %d] Hits for %d! Score: %d/%d\n", bat_id, runs_scored, global_score, wickets_fallen);

                if (runs_scored >= 4) {
                    ball_in_air = true;
                    pthread_cond_broadcast(&ball_in_air_cond);
                }
                
                if (runs_scored == 1 || runs_scored == 3) {
                    int temp = striker_id; striker_id = non_striker_id; non_striker_id = temp;
                }
                
                check_target(); 
                pthread_mutex_unlock(&score_mutex);
            }

            if (current_is_no_ball) active_free_hit = true; 
            else if (!current_is_wide && !current_is_no_ball) active_free_hit = false; 

            ball_ready = false; 
            pthread_cond_signal(&next_ball_cond); 
        }
        pthread_mutex_unlock(&pitch_mutex);
        usleep(5000); 
    }
    
    sem_post(&crease_sem);
    return NULL;
}

void* bowler_action(void* arg) {
    int bowler_id = *((int*)arg);
    int balls_this_spell = 0;
    
    while (balls_this_spell < 6 && total_balls_bowled < 120 && wickets_fallen < 10 && !match_over) {
        pthread_mutex_lock(&pitch_mutex);
        
        int extra_prob = rand() % 100;
        current_is_wide = (extra_prob < 5); 
        current_is_no_ball = (!current_is_wide && extra_prob < 10); 

        active_bowler_id = bowler_id; 

        if (!current_is_wide && !current_is_no_ball) {
            total_balls_bowled++;
            balls_this_spell++;
        }
        
        ball_ready = true; 
        pthread_cond_broadcast(&ball_bowled_cond); 
        
        while (ball_ready && !match_over) {
            pthread_cond_wait(&next_ball_cond, &pitch_mutex); 
        }
        
        if (balls_this_spell == 6 && !match_over && !current_is_wide && !current_is_no_ball) {
            int temp = striker_id; striker_id = non_striker_id; non_striker_id = temp;
            printf("\n--- End of over %d ---\n", total_balls_bowled / 6);
        }

        pthread_mutex_unlock(&pitch_mutex);
        usleep(10000); 
    }
    return NULL;
}