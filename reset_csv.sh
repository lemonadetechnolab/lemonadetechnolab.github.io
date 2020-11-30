#!/bin/sh

mv datalog_turb.csv data_history/
cp datalog_templ.csv datalog_turb.csv
cd data_history
mv datalog_turb.csv $(date +"%Y_%m_%d_%I_%M_%p")_datalog_turb.csv
