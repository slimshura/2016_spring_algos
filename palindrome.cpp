#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <utility>


using std::vector;
using std::string;

size_t char_to_number (char c){
    return c == 'A' ? 0:1;
}


vector<int> prefix_function (const string& str) {
	size_t strlen = str.length();
	vector <int> result (strlen, 0);
	for (size_t i=1; i<strlen; ++i) {
		int j = result[i-1];
		while (j > 0 && str[i] != str[j] )
			j = result[j-1];
		if (str[i] == str[j] )  ++j;
		result[i] = j;
	}
	return result;
}

vector <size_t> z_function (const string& str) {
	size_t strlen = str.length();
	vector <size_t> result (strlen, 0);
	size_t l=0, r=0;
	for (size_t i=1; i<strlen; ++i) {
		if (i <= r)
			result[i] = std::min (r-i+1, result[i-l]);
		while (i+result[i] < strlen && str[result[i]] == str[i+result[i]])
			++result[i];
		if ( i+result[i] > r+1){
			l = i;
			r = i+result[i]-1;
		}
	}
	return result;
}

size_t prefix_equal_to_reversed_pattern (const string & pattern){
    string::const_iterator itinc = pattern.cbegin();
    string::const_iterator itdesc = pattern.cend();
    size_t result=0;
    while( itinc != pattern.end() ) {
        --itdesc;
        if(*itinc == *itdesc) {
            ++itinc;
            ++result;
        }
        else
            break;
    }
    return result;
}

vector <bool> patternprefixispalindome (const string& pattern, const string& reversepattern) {
    string str = pattern+reversepattern;
    size_t patlen = pattern.length();
    vector <size_t> zfunc = z_function(str);
    vector <bool> result (patlen, false);
    result[0] = true;
    vector <bool>::iterator resultiter = result.begin();
    vector <size_t>::iterator ziter = zfunc.end();
    --ziter;
    for(size_t i=1; i<patlen; ++i) {
        --ziter;
        ++resultiter;
        if ( *ziter == i+1)
            *resultiter = true;
    }
    return result;
}


vector <vector <int> > prefpreprocessing (const string& pattern, const vector <int> & prefix_function_result) {
    vector <vector <int> > results (pattern.length(), vector <int>(2, 0) );
    size_t strlen = pattern.length();
    string::const_iterator stringiter = pattern.cbegin();
    vector <int>:: const_iterator pref_func_value_it = prefix_function_result.cbegin();
    if(pattern.front() == 'A' ) {
        results[0][0] = 1;
        results[0][1] = 0;
    }
    else{
        results[0][0] = 0;
        results[0][1] = 1;
    }
    for(size_t i=1; i<strlen; ++i) {
        ++stringiter;
        if (*stringiter == 'A'){
                results [i][0] = i+1;
                results [i][1] = results [*pref_func_value_it][1];
        }
        else{
            results [i][1] = i+1;
            results [i][0] = results [*pref_func_value_it][0];
        }
        ++ pref_func_value_it;
    }
    return results;
}


void makedinamicstep (size_t palindromelength,
                     size_t palindrome_prefix_equal_to_pattern_suffix,
                     size_t palindrome_suffix_equal_to_pattern_prefix,
                     const vector <vector  <long long> > & olddinamiccount,
                     vector <vector  <long long> > & newdinamiccount,
                     const vector <vector <int> > & prefprecounting,
                     const vector <vector <int> > & suffprecounting,
                     size_t patternlength
                     )
{
    size_t newpreflength = prefprecounting[palindrome_prefix_equal_to_pattern_suffix][0];
    size_t newsufflength = suffprecounting[palindrome_suffix_equal_to_pattern_prefix][0];
    if( (newpreflength < patternlength) && (newsufflength < patternlength) )
       {
           newdinamiccount [newpreflength] [newsufflength]+=
        olddinamiccount [palindrome_prefix_equal_to_pattern_suffix] [palindrome_suffix_equal_to_pattern_prefix];
       }
    newpreflength = prefprecounting[palindrome_prefix_equal_to_pattern_suffix][1];
    newsufflength = suffprecounting[palindrome_suffix_equal_to_pattern_prefix][1];
    if( (newpreflength < patternlength) && (newsufflength< patternlength) )
       {
           newdinamiccount [newpreflength] [newsufflength]+=
        olddinamiccount [palindrome_prefix_equal_to_pattern_suffix] [palindrome_suffix_equal_to_pattern_prefix];
       }
}


long long answer (const string& pattern, size_t palindromelength) {
    size_t patternlength = pattern.length();
    if(patternlength == 1) {
        return 1;
    }
    vector <vector <long long> > dinamiccount (patternlength, vector <long long> (patternlength, 0) );
    size_t len_residue_mod2 = palindromelength % 2;
    if (len_residue_mod2 == 0)
        dinamiccount[0][0] = 1;
    else {
        if(pattern.front() != pattern.back() ) {
            dinamiccount[0][0] = 0;
            dinamiccount[1][0] = 1;
            dinamiccount[0][1] = 1;
            dinamiccount[1][1] = 0;
        }
        else {
            dinamiccount[0][0] = 1;
            dinamiccount[1][0] = 0;
            dinamiccount[0][1] = 0;
            dinamiccount[1][1] = 1;
        }
    }
    string reversedpattern = pattern;
    reverse (reversedpattern.begin(), reversedpattern.end() );
    vector <int> preffunc = prefix_function (pattern);
    vector <int> sufffunc = prefix_function (reversedpattern);
    vector <vector <int> > suffprecounting = prefpreprocessing (pattern, preffunc);
    vector <vector <int> > prefprecounting = prefpreprocessing (reversedpattern, sufffunc);

    string str=reversedpattern+pattern;
    vector <int> big_pref_func_rev = prefix_function(str);
    str=pattern+reversedpattern;
    vector <int> big_pref_func = prefix_function(str);

    vector <bool> prefixpalindromes = patternprefixispalindome (pattern, reversedpattern);
    vector <bool> suffixpalindromes = patternprefixispalindome (reversedpattern, pattern);
    size_t prefix_is_reversed_suffix = prefix_equal_to_reversed_pattern (pattern);


    for (size_t cur_palindrome_length = len_residue_mod2; cur_palindrome_length+2 <= palindromelength;
                                                                            cur_palindrome_length+=2) {
        vector <vector <long long> > newdinamiccount (patternlength, vector <long long> (patternlength, 0) );
        for (size_t pal_suf_eq_to__pat_pref=0; pal_suf_eq_to__pat_pref < patternlength;
                                                            ++pal_suf_eq_to__pat_pref) {
                for (size_t pal_pref_eq_to__pat_suf=0; pal_pref_eq_to__pat_suf<patternlength;
                                                                ++pal_pref_eq_to__pat_suf) {
                    makedinamicstep (cur_palindrome_length, pal_suf_eq_to__pat_pref, pal_pref_eq_to__pat_suf,
                                              dinamiccount, newdinamiccount, prefprecounting, suffprecounting, patternlength);
                }
        }
        dinamiccount = std::move (newdinamiccount);
        bool call_both_sides_palindrome = false;
        bool call_pref_is_palind_and_suf_isnot = false;
        bool call_suf_is_palind_and_pref_isnot = false;
        if (cur_palindrome_length+2 <= patternlength ) {
                    if( prefixpalindromes[cur_palindrome_length+1] )
                            call_pref_is_palind_and_suf_isnot = true;
                    if ( suffixpalindromes[cur_palindrome_length+1] )
                            call_suf_is_palind_and_pref_isnot = true;
        }
        if (cur_palindrome_length+2 <= prefix_is_reversed_suffix &&
                (cur_palindrome_length+2 == patternlength ||
                  (prefixpalindromes[cur_palindrome_length+1] && suffixpalindromes[cur_palindrome_length+1] ) ) ) {
            call_both_sides_palindrome = true;
            call_pref_is_palind_and_suf_isnot = false;
            call_suf_is_palind_and_pref_isnot = false;
        }

        if (call_pref_is_palind_and_suf_isnot) {
            size_t oldsuf = big_pref_func_rev[patternlength+cur_palindrome_length+1];
            while (oldsuf > cur_palindrome_length+1)
                oldsuf=big_pref_func_rev[oldsuf-1];
            size_t oldpref = big_pref_func[cur_palindrome_length+1];
            dinamiccount[oldsuf][oldpref]--;
            dinamiccount[oldsuf][cur_palindrome_length+2]++;
        }

        if (call_suf_is_palind_and_pref_isnot) {
            size_t oldpref1 = big_pref_func[patternlength+cur_palindrome_length+1];
            while(oldpref1>cur_palindrome_length+1)
                oldpref1 = big_pref_func[oldpref1-1];
            size_t oldsuf1 = big_pref_func_rev[cur_palindrome_length+1];
            dinamiccount[oldsuf1][oldpref1]--;
            dinamiccount[cur_palindrome_length+2][oldpref1]++;
        }

        if (call_both_sides_palindrome) {
            size_t newpreflength = prefprecounting[ sufffunc[cur_palindrome_length] ][char_to_number (pattern.front() ) ];
            size_t newsufflength = suffprecounting[ preffunc[cur_palindrome_length] ][char_to_number (pattern.front() ) ];
            if( (newpreflength < patternlength) && (newsufflength < patternlength) )
                dinamiccount [newpreflength] [newsufflength]--;
            if (cur_palindrome_length+2 < patternlength)
                dinamiccount[cur_palindrome_length+2][cur_palindrome_length+2]++;
        }
    }


    long long result=0;
    for (size_t pal_suf_eq_to__pat_pref=0; pal_suf_eq_to__pat_pref < patternlength;
                                                        ++ pal_suf_eq_to__pat_pref) {
            for(size_t pal_pref_eq_to__pat_suff=0; pal_pref_eq_to__pat_suff < patternlength;
                                                                ++pal_pref_eq_to__pat_suff) {
                result+= dinamiccount[pal_suf_eq_to__pat_pref][pal_pref_eq_to__pat_suff];
            }
    }
    return result;
}

int main()
{
    size_t palindromelength;
    string pattern;
    std::cin >> palindromelength >> pattern;
    if( !pattern.length() ) {
        std::cout << 0 << std::endl;
        return 0;
    }
    std::cout << answer(pattern, palindromelength) << std::endl;
    return 0;
}
