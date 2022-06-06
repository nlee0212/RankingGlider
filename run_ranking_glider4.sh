./rg4-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz | tee -a rg4_output.txt
./rg4-config2 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz | tee -a rg4_output.txt
./rg4-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz | tee -a rg4_output.txt
./rg4-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz | tee -a rg4_output.txt
./rg4-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz | tee -a rg4_output.txt
./rg4-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz | tee -a rg4_output.txt
