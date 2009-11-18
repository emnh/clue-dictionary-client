const char accents[] = 
{

//there are some unresolved problems
//some locale capital letters are not made lowercase by tolower()
//such as æ, ø, å

//locale dependent case conversion
//'Æ', 'æ',
//'Ø', 'ø',
//'Å', 'å',

//sorting order fix, wrong for CLNOUKMX.DAT
'æ', 'å',
'ø', 'æ',
'å', 'ø',

//conflict
//'ö', 'æ', //norsk
'ö', 'o', //tysk

//accent removal
'à', 'a',
'á', 'a',
'â', 'a',
'ã', 'a',

//conflict
'ä', 'a', //tysk
//'ä', 'a', //svensk

//'å', 'a', //norsk
'ç', 'c',
'è', 'e',
'é', 'e',
'ê', 'e',
'ë', 'e',
'ì', 'i',
'í', 'i',
'î', 'i',
'ï', 'i',
'ñ', 'n',
'ò', 'o',
'ó', 'o',
'ô', 'o',
'õ', 'o',
//'ö', 'o', //svensk
'ù', 'u',
'ú', 'u',
'û', 'u',
'ü', 'u',
'ý', 'y',
'ÿ', 'y',
0, 0
};
