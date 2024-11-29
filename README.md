# Optimized COVID-19 Testing Group Creator: Project Overview  

This project focuses on efficient group-based COVID-19 testing using algorithmic optimization techniques to minimize testing costs while ensuring accurate detection. By analyzing social connection networks, it strategically groups individuals for testing, significantly reducing the number of tests needed.  

## Achievements  
The project was ranked *7th best* in the Algorithms and Data Structures course, a testament to its design and performance. It was implemented in *C++* and evaluated using an online portal where the solution earned points based on efficiency, accuracy, and runtime.  

## Key Features  
- *Algorithm Design:* Incorporates graph-based algorithms to assess social connections and determine optimal testing groups.  
- *Dynamic Group Sizing:* Implements logic to adjust group sizes dynamically based on infection probability and population size.  
- *Efficiency:* The solution reduces testing complexity from \( O(n^2) \) or higher in brute-force methods to a manageable \( O(n^3) \) in worst-case scenarios.  
- *Accuracy:* Ensures that the solution identifies all infected individuals within the bounds of the problem.  

## How It Works  
1. *Input Analysis:* Receives the number of people, initial infections, social connections, and infection spread probabilities.  
2. *Graph Modeling:* Models social connections using a graph, assigning weights based on the likelihood of virus transmission.  
3. *Score Calculation:* Calculates connection scores to prioritize grouping based on proximity and transmission risk.  
4. *Group Creation:* Dynamically creates testing groups, optimizing for both size and connection strength to reduce false positives/negatives.  
5. *Testing:* Efficiently tests groups or individuals based on real-time feedback to narrow down infected persons.  

## Highlights from Code Implementation  
- *Graph-Based Approach:* Uses adjacency maps for storing connection scores and levels.  
- *Recursive Scoring System:* Accounts for multi-level infection spread probability.  
- *Dynamic Group Testing:* Adapts group size based on infection bounds, balancing efficiency and accuracy.  
- *Memory Optimization:* Limits graph depth to ensure scalability for large populations.  

## Code Performance  
The program dynamically adjusts testing strategies based on input constraints, offering scalability while ensuring all edge cases are addressed. It integrates performance logging to measure testing efficiency during runtime.  

## Project Impact  
This solution demonstrates the power of computational thinking in real-world applications, showcasing how algorithms can save resources and improve outcomes in public health scenarios.
