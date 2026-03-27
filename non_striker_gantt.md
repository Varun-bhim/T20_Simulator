# 🏏 T20 Cricket Simulator - Match Analysis Charts

## 2007 ICC T20 World Cup Final: India vs Pakistan

**Simulation Generated**: Multi-threaded C program using OS scheduling concepts  
**Match Format**: 20 overs per side | **Venue**: Johannesburg, South Africa | **Date**: September 24, 2007

---

## 📊 Available Visualizations

### 1. **[Striker Batsman Timeline](striker_gantt.md)** 🏟️
Track which batsman was actively **facing the ball** from the bowler

- Shows the striker at the crease per delivery
- Indicates who was in control/attacking position
- Reveals batting dominance patterns
- **Focus**: Aggressive batting phase tracking

**Quick Access**: [View Striker Chart →](striker_gantt.md)

---

### 2. **[Non-Striker Batsman Timeline](non_striker_gantt.md)** 🤝
Visualize which batsman was **standing at the other end**, waiting for their turn

- Shows all batsmen rotations and partnerships
- Reveals sustained partnerships and quick singles
- Tracks player availability and participation
- **Focus**: Partnership stability & distribution

**Key Insight**: Longer bars = more stable partnerships  
**Quick Access**: [View Non-Striker Chart →](non_striker_gantt.md)

---

### 3. **[Bowler Rotation & Spell Analysis](bowler_gantt.md)** 🎯
Understand the **bowling strategy** - which bowlers bowled when and for how long

- Shows each bowler's spells throughout the innings
- Reveals tactical rotations and death-over strategies
- Tracks workload distribution among 5 bowlers
- **Focus**: Bowling strategy & team tactics

**Key Insight**: Spell changes indicate strategic decisions  
**Quick Access**: [View Bowler Chart →](bowler_gantt.md)

---

## 🎯 How to Use These Charts

### Reading the Timeline Format

```
Delivery Number: 1 ——— 30 ——— 60 ——— 90 ——— 120+
                 ↑           ↑             ↑
              Over 1      Over 5        Over 20
              (Overs vary with wides/no-balls)
```

### Key Data Points

| Chart | Shows | Best For |
|-------|-------|----------|
| **Striker** | Active batsmen at crease | Identifying aggressive phases |
| **Non-Striker** | Waiting batsmen | Understanding partnerships & rotations |
| **Bowler** | Bowling spells | Analyzing tactical strategies |

---

## 📈 Match Structure Breakdown

### T20 Cricket Basics
- **Per Innings**: 20 overs = 120 legal deliveries (+ extras)
- **Bowling teams**: 5 specialist bowlers
- **Batting side**: 11 players (1-2 on crease, rest in pavilion)
- **Over structure**: 6 deliveries = 1 over (bowler changes after each over)

### Delivery Count vs Over Count
- **120 legal balls** = 20 overs (standard)
- **Wides & no-balls** = count as deliveries but NOT as overs
- **Total deliveries** shown in charts = legal balls + extras

---

## 🔍 Analysis Patterns to Spot

### From Striker Chart
- ✅ **Confident batting**: Few rotations, longer strike time
- ✅ **Aggressive phase**: Frequent boundary hits (implied by quick delivery progression)
- ⚠️ **Pressure from bowlers**: More defensive, slower score progression

### From Non-Striker Chart
- ✅ **Stable opening partnership**: Long bars at top of chart
- ✅ **Aggressive batsman support**: Frequent changes indicate good runners
- ⚠️ **Collapse phase**: Rapid changes indicate quick wickets

### From Bowler Chart
- ✅ **Balanced attack**: All bowlers have similar spell counts
- ✅ **Death-over strategy**: Specialist bowlers in overs 17-20
- ⚠️ **Workload issue**: One bowler overburdened with multiple long spells

---

## 💡 Simulation Features

### Scheduling Modes
**FCFS (First-Come-First-Served)**
```bash
./t20_simulator
```
- Normal batting order with death-over specialist priority in last 6 balls
- Reflects traditional cricket strategy

**SJF (Shortest-Job-First)**
```bash
./t20_simulator sjf
```
- Tail-enders receive priority for batting opportunities
- Demonstrates alternative tactical approach
- Shows impact of non-traditional batting order

### Data Tracking
- **OS Clock**: System ticks tracking delivery sequence
- **Thread Synchronization**: Real-time batsman/bowler coordination
- **Extras Handling**: Wides & no-balls properly counted in timeline
- **Match State**: Dynamic toss, innings tracking, target calculation

---

## 🏆 How to Interpret Results

### Winning Strategy
Look for:
1. **Striker chart**: Extended periods of dominance
2. **Non-striker chart**: Stable partnerships (fewer changes)
3. **Bowler chart**: Varied attacks preventing adaptation

### Chasing Performance (Innings 2)
Evaluate:
1. How quickly chasing team reached target
2. Whether any bowler was particularly expensive
3. Pattern of wickets vs runs conceded

---

## 📋 Chart Statistics Summary

All charts automatically generated from live simulation data:
- **Accuracy**: Delivery-by-delivery tracking
- **Completeness**: All 40 overs (both innings) mapped
- **Detail Level**: Includes legal balls AND extras
- **Refresh**: Regenerated each simulation run

---

## 🔗 Related Documentation

- **Match Rules**: See C source files for detailed logic
- **Simulation Code**: Check `main.c`, `match.c`, `threads.c`
- **Scheduling Logic**: Review `threads.c` for batsman dispatch
- **Scoring System**: Refer to ball outcome probability in `threads.c`

---

## 📊 Quick Reference

| File | Content | Updated |
|------|---------|---------|
| `striker_gantt.md` | Striker timeline | ✅ Auto-generated |
| `non_striker_gantt.md` | Non-striker timeline | ✅ Auto-generated |
| `bowler_gantt.md` | Bowler rotation | ✅ Auto-generated |

---

**Generated by**: T20 Cricket Simulator v2.0  
**Engine**: Multi-threaded C with OS scheduling concepts  
**Simulation Date**: Real-time execution based on random seed  
**Purpose**: Educational - Demonstrating thread coordination & scheduling algorithms in sports context

