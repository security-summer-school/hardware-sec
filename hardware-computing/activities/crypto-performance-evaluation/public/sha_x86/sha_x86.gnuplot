set title 'x86 SHA Hardware and Software Comparison'
set xlabel 'Input Data (bytes)'
set ylabel 'Processed Data (bytes)'
set terminal png
set output 'sha_x86.png'
set logscale
set logscale y

plot 'sha_x86.csv' using 1:2 title 'x86 Software SHA1' with lines, 'sha_x86.csv' using 1:3 title 'x86 Hardware SHA1' with lines, 'sha_x86.csv' using 1:4 title 'x86 Software SHA256' with lines, 'sha_x86.csv' using 1:5 title 'x86 Hardware SHA256' with lines
