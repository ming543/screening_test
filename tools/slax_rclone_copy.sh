#rclone sync /home/production/u7_log onedrive:General/u7_log -P
USB30="/home/production/slax"
ONE="onedrive:General/EagleEyes_Test/AIM_BI"
sudo rclone copy $ONE/slax $USB30/slax -P
sudo rclone copy $ONE/HW_test_script $USB30/HW_test_script -P
sudo rclone copy $ONE/logfile $USB30/logfile -P

