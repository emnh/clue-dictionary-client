const char accents[] = 
{

//there are some unresolved problems
//some locale capital letters are not made lowercase by tolower()
//such as �, �, �

//locale dependent case conversion
//'�', '�',
//'�', '�',
//'�', '�',

//sorting order fix, wrong for CLNOUKMX.DAT
'�', '�',
'�', '�',
'�', '�',

//conflict
//'�', '�', //norsk
'�', 'o', //tysk

//accent removal
'�', 'a',
'�', 'a',
'�', 'a',
'�', 'a',

//conflict
'�', 'a', //tysk
//'�', 'a', //svensk

//'�', 'a', //norsk
'�', 'c',
'�', 'e',
'�', 'e',
'�', 'e',
'�', 'e',
'�', 'i',
'�', 'i',
'�', 'i',
'�', 'i',
'�', 'n',
'�', 'o',
'�', 'o',
'�', 'o',
'�', 'o',
//'�', 'o', //svensk
'�', 'u',
'�', 'u',
'�', 'u',
'�', 'u',
'�', 'y',
'�', 'y',
0, 0
};
