#!/bin/sh

mv status.html data_history/
cp status_templ.html status.html
cd data_history
mv status.html $(date +"%Y_%m_%d_%I_%M_%p")_orders.html
