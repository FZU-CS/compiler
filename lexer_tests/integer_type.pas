{ Program for testing Pascal Lexer }

t := 3;
a := 0011; {2}
b := 0111; {8}
c := 0x11; {16}
d := 0023; {8}
e := 0087; {10}
f := 00AA; {16}
error := 00FZ; {error, skip this line}

long_bin_number := 00101010111010;
long_bin_number_with_point := 00101010111010.01010101;

long_oct_number_1 := 00102345077211;
long_oct_number_2 := 07121072154212234;
long_oct_number_3 := 00000000000117721;
long_oct_number_with_point := 00000000000117721.7717257;

long_dec_number_1 := 777777218217272121213455;
long_dec_number_2 := 0012716713182172711;
long_dec_number_3 := 00000000121283189;
long_dec_number_with_point := 123456789123456789.123456789;

long_hex_number := 0x0A000EF00ef1;
long_hex_number_with_point := 0x0A000EF00ef1.fffffffaeee;