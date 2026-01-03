#ifndef PROFILER_H
#define PROFILER_H

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct ProfileEntry {
    uint32_t pc;
    uint64_t count;
    uint64_t total_cycles;
    ProfileEntry() : pc(0), count(0), total_cycles(0) {}
    ProfileEntry(uint32_t p, uint64_t c, uint64_t tc) 
        : pc(p), count(c), total_cycles(tc) {}
};

class Profiler {
public:
    Profiler() : total_instructions(0), profiling_enabled(true) {}
    
    // Record instruction execution
    void record_instruction(uint32_t pc) {
        if (!profiling_enabled) return;
        
        instruction_counts[pc]++;
        total_instructions++;
    }
    
    // Get hot instructions (executed most frequently)
    std::vector<ProfileEntry> get_hot_instructions(size_t top_n = 10) const {
        std::vector<ProfileEntry> entries;
        
        for (const auto& [pc, count] : instruction_counts) {
            entries.emplace_back(pc, count, count); // cycles = count for now
        }
        
        // Sort by execution count
        std::sort(entries.begin(), entries.end(), 
                  [](const ProfileEntry& a, const ProfileEntry& b) {
                      return a.count > b.count;
                  });
        
        // Return top N
        if (entries.size() > top_n) {
            entries.resize(top_n);
        }
        
        return entries;
    }
    
    // Detect hot loops (basic block that jumps back to itself)
    std::vector<uint32_t> detect_hot_loops(uint64_t threshold = 100) const {
        std::vector<uint32_t> hot_loops;
        
        for (const auto& [pc, count] : instruction_counts) {
            if (count >= threshold) {
                hot_loops.push_back(pc);
            }
        }
        
        return hot_loops;
    }
    
    // Statistics
    uint64_t get_total_instructions() const { return total_instructions; }
    uint64_t get_unique_instructions() const { return instruction_counts.size(); }
    
    double get_instruction_percentage(uint32_t pc) const {
        auto it = instruction_counts.find(pc);
        if (it == instruction_counts.end() || total_instructions == 0) {
            return 0.0;
        }
        return (static_cast<double>(it->second) / total_instructions) * 100.0;
    }
    
    // Control
    void enable_profiling() { profiling_enabled = true; }
    void disable_profiling() { profiling_enabled = false; }
    void reset() {
        instruction_counts.clear();
        total_instructions = 0;
    }
    
    // Display results
    void print_profile() const;
    
private:
    std::unordered_map<uint32_t, uint64_t> instruction_counts;
    uint64_t total_instructions;
    bool profiling_enabled;
};

#endif // PROFILER_H