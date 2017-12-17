while a <> b do  WriteLn('Waiting');

if a > b then WriteLn('Condition met')   {no semicolon allowed!}
           else WriteLn('Condition not met');

for i := 1 to 10 do  {no semicolon for single statements allowed!}
  WriteLn('Iteration: ', i);

repeat
  a := a + 1
until a = 10;

case i of
  0 : Write('zero');
  1 : Write('one');
  2 : Write('two');
  3,4,5,6,7,8,9,10: Write('?')
end;