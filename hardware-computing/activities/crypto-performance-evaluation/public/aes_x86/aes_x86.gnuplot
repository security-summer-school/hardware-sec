set title 'x86 AES Hardware and Software Comparison'
set xlabel 'Input Data (bytes)'
set ylabel 'Processed Data (bytes)'
set terminal png
set output 'aes_x86.png'
set logscale
set logscale y

plot 'aes_x86.csv' using 1:2 title 'x86 Software AES' with lines, 'aes_x86.csv' using 1:3 title 'x86 Hardware AES' with lines


