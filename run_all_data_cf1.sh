
./rg3-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/graph_analytics_10M.trace.gz
./rg3-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/libquantum_10M.trace.gz
./rg3-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/mcf_10M.trace.gz
./rg3-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/xalancbmk_10M.trace.gz

./hawkeye-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/graph_analytics_10M.trace.gz
./hawkeye-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/libquantum_10M.trace.gz
./hawkeye-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/mcf_10M.trace.gz
./hawkeye-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/xalancbmk_10M.trace.gz

./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/graph_analytics_10M.trace.gz
./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/libquantum_10M.trace.gz
./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/mcf_10M.trace.gz
./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/xalancbmk_10M.trace.gz

./srrip-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/graph_analytics_10M.trace.gz
./srrip-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/libquantum_10M.trace.gz
./srrip-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/mcf_10M.trace.gz
./srrip-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/xalancbmk_10M.trace.gz
