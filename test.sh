
for i in 1 2 3 4 5; do

  rm -f /tmp/results${i}.txt

  for pol in FCFS SSTF SCAN C-SCAN; do
    echo -n "${pol} -> " >>/tmp/results${i}.txt
    ./dscheduler ${pol} < test/test$i.txt >>/tmp/results${i}.txt
  done

  diff -qb test/results${i}.txt /tmp/results${i}.txt
  if [ $? -eq 0 ]; then
    echo "Test $i PASS"
  else
    echo "Test $i FAIL"
  fi
  rm /tmp/results${i}.txt
done
