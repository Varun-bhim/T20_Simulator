# T20 WC Cricket Simulator (OS Threading Assignment)

## Project Overview
This is a multi-threaded T20 cricket simulator designed for an Operating Systems course assignment (CSC-204). It models a full match between India and Pakistan using thread-based players (batsmen, bowler, fielders), mutexes, semaphores, and condition variables.

The simulation reproduces:
- Ball-by-ball action and score updates
- Wickets, runs, wides, no-balls, free hits
- Innings transition and match result
- Gantt charts for striker/non-striker/bowler thread usage
- Wait-time analysis (FCFS vs SJF scheduling concepts)

## Deliverables Verification
1. **Code (Ball-by-ball log)**
   - Implemented in `threads.c` (`batsman_action`, `bowler_action`, `fielder_action`)
   - Logs include:
     - Bowler deliveries and extras
     - Runs/wickets/free-hit state
     - Target checks and match conclusion

2. **Gantt Chart Visualization**
   - `generate_gantt_chart()` in `match.c` (pitch/thread usage; optional but present)
   - `generate_striker_gantt_chart()` in `match.c` (writes `striker_gantt.md`)
   - `generate_non_striker_gantt_chart()` in `match.c` (writes `non_striker_gantt.md`)
   - `generate_bowler_gantt_chart()` in `match.c` (writes `bowler_gantt.md`)

3. **Scheduling Analysis**
   - `print_wait_time_analysis()` in `match.c`: detailed OS scheduling comparison and conclusion.

## 🛠️ Build and Run
Use GCC + pthreads (Makefile included).

```bash
make
./t20_simulator          # FCFS default behavior
./t20_simulator sjf      # SJF mode (tail-enders priority)
```

### Outputs
- `gantt.md` (overall pitch usage)
- `striker_gantt.md`, `non_striker_gantt.md`, `bowler_gantt.md`
- Console: scorecard, flow details, result

## 🔧 Key Structures and Files
- `globals.c` / `simulator.h`: global state, squad definitions, shared arrays
- `main.c`: match flow, toss, innings orchestration
- `match.c`: core cricket logic, state reset, Gantt chart generation, analysis
- `threads.c`: threaded behavior of bowlers/batsmen/fielders

## Important Behavior Notes
- Bowler Gantt chart counts **all deliveries** (legal + wides + no-balls), so a bowler can appear to have 7+ sequential deliveries in a spell if there are extras. This explains why it may show 1-6 for Asif then 7-13 for Tanvir.
- `total_balls_bowled` increments only on legal balls; `total_deliveries` increments on every delivery (including extras).

## Task/Feature Checklist
- [x] Thread synchronization (mutex, cond var, semaphore)
- [x] One bowler on pitch at a time
- [x] Batsman waits for ball signal
- [x] Fielders wake on `ball_in_air_cond`
- [x] SJF/FCFS scheduling mechanism implemented (`use_sjf_scheduling` & `spawn_order`)
- [x] Step-by-step match state and score update
- [x] 2 innings run and target logic with win/tie output
- [x] Gantt chart generation
- [x] Wait time analysis logic

## Enhancement Suggestions
- Separate Gantt for legal balls only (if charting by over rather than delivery count).
- Add CSV / JSON export for analytics.
- Add feedback of computed average wait time numbers in `print_wait_time_analysis()`.

---

### Final Assessment
All requested deliverables are implemented and working. The only behavior that might look unexpected (`S. Tanvir: 7-13`) is because the chart is based on every delivery including extras, which is consistent with current code and the attached chart data.
