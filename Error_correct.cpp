#include "stdafx.h"

//#include "stdio.h"
//#include "math.h"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "config.h"
#include "ppm.h"
#include "spmf.h"
#include "mrn.h"
//#include "frn.h"

#include "utilities.h" 
#include "error_correct.h"

void int_correct(ECC* ecc);
void correct_add_demo(ECC* ecc);
void correct_sub_demo(ECC* ecc);
void correct_mult_demo(ECC* ecc);
void fixed_point_correct_demo1(ECC* ecc);


void print_note_1(void);
int get_digit_position(SPPM *sppm);
int get_digit_value(int dig_pos);
int get_next_modulus(int small_modulus);		// gets the next largest modulus
int get_value(SPPM* arg, string s);			// get a signed integer SPPM value from user, send prompt over
int get_fp_value(SPMF* arg, string s);		// get a fixed point SPMF value from user, send prompt over
ERROR_CODES correct_digit(SPPM* val, ECC* ecc);


// print menu for RNS demo example
void correction_menu(void)
{

	printf("RNS ARITHMETIC ERROR CORRECTION Demo Menu\n");
	cout << "Courtesy of Maitrix.com" << endl << endl;
	printf("1. error correct a signed integer		BASIC\r\n");
	printf("2. error corrected addition			BASIC\r\n");
	printf("3. error corrected subtraction			BASIC\r\n");
	printf("4. error corrected integer multiply		BASIC\r\n");
	printf("5. error corrected fixed-point multiply		INTERMEDIATE\r\n");
	printf("6. exit to RNS demo\r\n");

}

// demo routine to illustrate RNS concepts
void correction_tests(void)
{
	char c[100];
	char* s;

	print_note_1();

	ECC* ecc = new (ECC_t);			// allocate error correction constants structure
	generate_ecc_constants(ecc);	// generate the error correction constants (via a pointer to ECC_t structure)  (only need to do this once)

	cout << "the value of the Mt: " << ecc->Mt->Print(10) << endl;
	cout << "the value of the Mt/2: " << ecc->M_half->Print(10) << endl << endl;

	cout << "hit ENTER to begin the Demo" << endl;
	wait_key();


	c[0] = 0;
	s = &c[0];
	while (strcmp(s, "q")) {

		cls();

		correction_menu();

		printf("\r\nEnter choice: ");

		scanf("%s", &c[0]);


		switch (c[0]) {
		case '1':
			cls();
			int_correct(ecc);
			wait_key();
			break;
		case '2':
			cls();
			correct_add_demo(ecc);
			wait_key();
			break;
		case '3':
			cls();
			correct_sub_demo(ecc);
			wait_key();
			break;
		case '4':
			cls();
			correct_mult_demo(ecc);
			wait_key();
			break;
		case '5':
			cls();
			fixed_point_correct_demo1(ecc);
			wait_key();
			break;
		case '6':
			cls();
			cout << "leaving error correction Demo" << endl;
			return;
//			wait_key();
			break;
		default:
			printf("Invalid menu key, try again\r\n");
			wait_key();
			break;
		}

		//		wait_key();
	}

}


// this fixed-point demo shows how an intermediate product can be corrected;
// in case of performing product summations, the same technique is employed.
// This technique corrects the final un-normalized product before performing a final
// normalization of the Intermediate Product.
void fixed_point_correct_demo1(ECC* ecc)
{
	SPMF* spmf1 = new SPMF(0);
	SPMF* spmf2 = new SPMF(0);
	SPMF* spmf_temp = new SPMF(0);
	enum ERROR_CODES ret_error;
	int digit_index, digit_value;
	char c;

	spmf_temp->PPM::Assign(ecc->M_half);


	while (1) {

		cout << "RNS ERROR CORRECTED FIXED-POINT MULTIPLY DEMO" << endl;
		cout << "for multiplication, enter two numbers less than: (+/-) " << spmf_temp->Print(10) << endl << endl;
		cout << "whose product does not exceed (+/-) " << spmf_temp->Print(10) << endl;
		cout << "hit ENTER to continue" << endl;
		wait_key();

		get_fp_value(spmf1, "Enter first fixed-pont value (ex: 23.456)");
		get_fp_value(spmf2, "Enter second fixed-pont value (ex: -89.765)");

		spmf_temp->Assign(spmf1);
		spmf_temp->MultStd(spmf2);

		cout << "you have entered: " << endl;
		cout << spmf1->Print(10) << " * " << spmf2->Print(10) << " = " << spmf_temp->Print(10) << endl << endl;

		cout << "hit ENTER to continue" << endl;
		wait_key();

		digit_index = get_rand(0, spmf1->NumDigits - 1);

		cout << "randomly selecting digit index " << digit_index << " and changing its value." << endl;
		cout << "the correct digit value is: " << spmf1->Rn[digit_index]->Digit << endl;

		while ((digit_value = get_rand(0, spmf1->Rn[digit_index]->GetPowMod2() - 1)) == spmf1->Rn[digit_index]->Digit);
		cout << "the randomly chosen digit is: " << digit_value << endl;

		cout << "hit ENTER key to see how this error affects the fixed-point multiply operation" << endl;
		wait_key();

		spmf1->Rn[digit_index]->Digit = digit_value;
		spmf_temp->Assign(spmf1);
		spmf_temp->MultStd(spmf2);

		cout << spmf1->Print(10) << " * " << spmf2->Print(10) << " = " << spmf_temp->Print(10) << endl << endl;

		cout << "the RESULT is no longer correct !!" << endl << endl;
		cout << "hit ENTER key to run correction algorithm on INTERMEDIATE PRODUCT RESULT" << endl;
		cout << "then normalization will be re-applied to generate a corrected result" << endl;
		cout << "this same process may be applied to a fixed-point product summation" << endl;
		wait_key();
		wait_key();

		spmf_temp->Assign(spmf1);
		spmf_temp->SPPM::Mult(spmf2);		// repeat calc:  need to create an intermediate product in error (splitting multiply into two)

		ret_error = correct_digit(spmf_temp, ecc);		// SPMF is based on SPPM

		spmf_temp->I2N_Convert();			// normalize the corrected intermediate product

		cout << "corrected result = " << spmf_temp->Print(10) << endl << endl;

		cout << "hit ENTER to continue or 'q' to exit" << endl;
		c = wait_key();

		if (c == 'q') break;

	}


}


void correct_mult_demo(ECC* ecc)
{
	int digit_index, digit_value;
	SPPM* sppm1 = new SPPM(0);
	SPPM* sppm2 = new SPPM(0);
	SPPM* sppm_temp = new SPPM(0);
	enum ERROR_CODES ret_error;
	char c;


	init_rand();

	while (1) {

		cout << "RNS ERROR CORRECTED MULTIPLY DEMO" << endl;
		cout << "for multiplication, enter two signed integers" << endl;
		cout << "whose product is less than (+/-) " << ecc->M_half->Print(10) << "-1 " << endl << endl;

		if (get_value(sppm1, "enter the first number or 'q' to quit (ex: -1234):") == -1) break;
		get_value(sppm2, "enter the second number (ex: 76589):");
		sppm_temp->Assign(sppm1);
		sppm_temp->Mult(sppm2);			// perform signed integer multiply first as a check

		cout << "you have entered: " << endl;
		cout << sppm1->Print(10) << " * " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "hit ENTER to continue" << endl;
		wait_key();

		digit_index = get_rand(0, sppm1->NumDigits - 1);

		cout << "randomly selecting digit index " << digit_index << " and changing its value." << endl;
		cout << "the correct digit value is: " << sppm1->Rn[digit_index]->Digit << endl;

		while ((digit_value = get_rand(0, sppm1->Rn[digit_index]->GetPowMod2() - 1)) == sppm1->Rn[digit_index]->Digit);
		cout << "the randomly chosen digit is: " << digit_value << endl;

		cout << "hit ENTER key to see how this error affects the multiply operation" << endl;
		wait_key();

		sppm1->Rn[digit_index]->Digit = digit_value;
		sppm_temp->Assign(sppm1);
		sppm_temp->Mult(sppm2);			// perform regular addition again with arg in error

		cout << sppm1->Print(10) << " * " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "the RESULT is no longer correct !!" << endl << endl;
		cout << "hit ENTER key to run correction algorithm on RESULT" << endl;
		wait_key();
		wait_key();

		ret_error = correct_digit(sppm_temp, ecc);

		if (ret_error == NO_ERROR) {
			cout << "no error detected, product is:" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "raw RNS value is still the same:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else if (ret_error == SINGLE_ERROR) {
			cout << "single digit error detected, the product RESULT is corrected :" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "the corrected raw RNS product is:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else {
			cout << "error detected, but product RESULT not corrected\n" << endl;		// for single digit error shouldn't get here
		}

		cout << "hit ENTER to continue or 'q' to exit" << endl;
		c = wait_key();

		if (c == 'q') break;
	}

}


void correct_sub_demo(ECC* ecc)
{
	int digit_index, digit_value;
	SPPM* sppm1 = new SPPM(0);
	SPPM* sppm2 = new SPPM(0);
	SPPM* sppm_temp = new SPPM(0);
	enum ERROR_CODES ret_error;
	char c;


	init_rand();

	while (1) {

		cout << "RNS ERROR CORRECTED SUBTRACTION DEMO" << endl;
		cout << "for subtraction, enter two numbers less than: (+/-) " << ecc->M_half->Print(10) << "-1 " << endl << endl;
		cout << "whose difference is also less than (+/-) " << ecc->M_half->Print(10) << endl;

		if (get_value(sppm1, "enter the first number or 'q' to quit (ex: -1234):") == -1) break;
		get_value(sppm2, "enter the second number (ex: 76589):");
		sppm_temp->Assign(sppm1);
		sppm_temp->Sub(sppm2);			// perform regular subtraction first as a check

		cout << "you have entered: " << endl;
		cout << sppm1->Print(10) << " - " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "hit ENTER to continue" << endl;
		wait_key();


		digit_index = get_rand(0, sppm1->NumDigits - 1);

		cout << "randomly selecting digit index " << digit_index << " and changing its value." << endl;
		cout << "the correct digit value is: " << sppm1->Rn[digit_index]->Digit << endl;

		while ((digit_value = get_rand(0, sppm1->Rn[digit_index]->GetPowMod2() - 1)) == sppm1->Rn[digit_index]->Digit);
		cout << "the randomly chosen digit is: " << digit_value << endl;

		cout << "hit ENTER key to see how this error affects the sum operation" << endl;
		wait_key();

		sppm1->Rn[digit_index]->Digit = digit_value;
		sppm_temp->Assign(sppm1);
		sppm_temp->Sub(sppm2);			// perform regular addition again with arg in error

		cout << sppm1->Print(10) << " - " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "the RESULT is no longer correct !!" << endl << endl;
		cout << "hit ENTER key to run correction algorithm on RESULT" << endl;
		wait_key();
		wait_key();

		ret_error = correct_digit(sppm_temp, ecc);

		if (ret_error == NO_ERROR) {
			cout << "no error detected, difference is:" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "raw RNS value is still the same:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else if (ret_error == SINGLE_ERROR) {
			cout << "single digit error detected, the RESULT is corrected :" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "the corrected raw RNS difference is:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else {
			cout << "error detected, but subtraction RESULT not corrected\n" << endl;		// for single digit error shouldn't get here
		}

		cout << "hit ENTER to continue or 'q' to exit" << endl;
		c = wait_key();

		if (c == 'q') break;
	}

}


void correct_add_demo(ECC* ecc)
{
	int digit_index, digit_value;
	SPPM* sppm1 = new SPPM(0);
	SPPM* sppm2 = new SPPM(0);
	SPPM* sppm_temp = new SPPM(0);
	enum ERROR_CODES ret_error;
	char c;


	init_rand();

	while (1) {

		cout << "RNS ERROR CORRECTED ADDITION DEMO" << endl;
		cout << "enter two numbers whose sum is less than: (+/-) " << ecc->M_half->Print(10) << "-1 " << endl << endl;

		if(get_value(sppm1, "enter the first number or 'q' to quit (ex: -1234):") == -1) break;
		get_value(sppm2, "enter the second number (ex: 76589):");
		sppm_temp->Assign(sppm1);
		sppm_temp->Add(sppm2);			// perform regular addition first as a check

		cout << "you have entered: " << endl;
		cout << sppm1->Print(10) << " + " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "hit ENTER to continue" << endl;
		wait_key();


		digit_index = get_rand(0, sppm1->NumDigits - 1);

		cout << "randomly selecting digit index " << digit_index << " and changing its value." << endl;
		cout << "the correct digit value is: " << sppm1->Rn[digit_index]->Digit << endl;

		while ((digit_value = get_rand(0, sppm1->Rn[digit_index]->GetPowMod2() - 1)) == sppm1->Rn[digit_index]->Digit);
		cout << "the randomly chosen digit is: " << digit_value << endl;

		cout << "hit ENTER key to see how this error affects the sum operation" << endl;
		wait_key();
		wait_key();

		sppm1->Rn[digit_index]->Digit = digit_value;
		sppm_temp->Assign(sppm1);
		sppm_temp->Add(sppm2);			// perform regular addition again with arg in error

		cout << sppm1->Print(10) << " + " << sppm2->Print(10) << " = " << sppm_temp->Print(10) << endl << endl;

		cout << "the RESULT is no longer correct !!" << endl << endl;
		cout << "hit ENTER key to run correction on result" << endl;
		wait_key();

		ret_error = correct_digit(sppm_temp, ecc);

		if (ret_error == NO_ERROR) {
			cout << "no error detected, sum value is:" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "raw RNS value is still the same:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else if (ret_error == SINGLE_ERROR) {
			cout << "single digit error detected, and the result is corrected :" << endl;
			cout << sppm_temp->Print(10) << endl;
			cout << "the corrected raw RNS summation is:" << endl;
			sppm_temp->PrintDemoPM(10);
		}
		else {
			cout << "error detected, but summation not corrected\n" << endl;		// for single digit error shouldn't get here
		}

		cout << "hit ENTER to continue or 'q' to exit" << endl;
		c = wait_key();

		if (c == 'q') break;
	}

}

void int_correct(ECC* ecc)
{
char input[100];
string inval;
enum ERROR_CODES ret_error;
int dig_pos, digit;
char c;


	SPPM *testval = new SPPM(0);

	while (1) {

		cout << "SIMPLE RNS VALUE CORRECTION DEMO" << endl;
		cout << "the maximum correctable value is: (+/-) " << ecc->M_half->Print(10) << "-1 " << endl;
		cout << "Enter signed integer value or 'q' to quit (Example: -1234): ";
		cin >> inval;

		if (inval == "q") {
			cout << "quiting..." << endl;
			break;
		}

		cout << endl;

		testval->Assign(inval);

		cout << "value entered in decimal: " << testval->Print(10) << endl;
		cout << "raw RNS value entered is:" << endl << endl;
		testval->PrintDemoPM(10);
		cout << "note the RNS number above and hit ENTER to change a digit" << endl;
		cout << "(you will create a number that has an error in one of its digits)" << endl;
		wait_key();

		dig_pos = get_digit_position(testval);
		cout << "the value of this digit is currently: " << testval->Rn[dig_pos]->Digit << endl;

		digit = get_digit_value(dig_pos);

		if (digit < 0) {
			// problem
		}

		testval->Rn[dig_pos]->Digit = digit;

		cout << "your new modified number is now = " << testval->Print(10) << endl;
		cout << "your new modified number in RNS :" << endl << endl;
		testval->PrintDemoPM(10);

		cout << "press ENTER to error correct the value..." << endl << endl;
		wait_key();
		wait_key();

		ret_error = correct_digit(testval, ecc);

		if (ret_error == NO_ERROR) {
			cout << "no error detected, value is:" << endl;
			cout << testval->Print(10) << endl;
			cout << "raw RNS value is still the same:" << endl;
			testval->PrintDemoPM(10);
		}
		else if (ret_error == SINGLE_ERROR) {
			cout << "single digit error detected, and the value corrected :" << endl;
			cout << testval->Print(10) << endl;
			cout << "the corrected raw RNS value is:" << endl;
			testval->PrintDemoPM(10);
		}
		else {
			cout << "error detected, but value not corrected\n" << endl;		// for single digit error shouldn't get here
		}

		cout << "hit ENTER to continue or 'q' to exit" << endl;
		c = wait_key();

		if (c == 'q') break;
	}
}

void print_note_1(void)
{

	cout << "BASIC RNS ERROR CORRECTION DEMO ROUTINE" << endl;
	cout << "Programmed and Invented by Eric Olsen" << endl;
	cout << "Courtesy of Maitrix.com" << endl << endl;

	cout << "This code licensed under https://creativecommons.org/licenses/by-nc-sa/4.0/" << endl;
	cout << "These demo routines demonstrate error correction of RNS values encoded using method of complements;" << endl;
	cout << "This is also equivalent to detecting an error in the result of an RNS arithmetic operation" << endl;
	cout << "since an error in one digit position does not propagate to another digit position for (+/-/*)." << endl;
	cout << "To learn more, see https://maitrix.com/resources/publication/ " << endl;
	cout << "For commercial licensing, contact info@maitrix.com" << endl;
	cout << endl;

}

// Assign has a flaw, needs to return an error code if illegal input
int get_value(SPPM* arg, string s)
{
	string inval;

	cout << s;
	cin >> inval;

	if (inval == "q") {
		cout << "quiting..." << endl;
		return(-1);
	}

	arg->Assign(inval);
	return(0);

}

// Assign has a flaw, needs to return an error code if illegal input
int get_fp_value(SPMF* arg, string s)
{
	string inval;

	cout << s;
	cin >> inval;

	if (inval == "q") {
		cout << "quiting..." << endl;
		return(-1);
	}

	arg->AssignFP(inval);
	return(0);

}



// this routine prompts user to select one of the RNS number system digits
int get_digit_position(SPPM *sppm1)
{
//PPM *ppm1 = new PPM(0);
string inval;
int i, digit;

	cout << endl;
	cout << "This RNS system has " << sppm1->NumDigits << " digits to choose from:" << endl << endl;

	for (i = 0; i < sppm1->NumDigits; i++) {
		cout << "index " << i << " : " << sppm1->Rn[i]->Digit << " \t";
		if (!((i+1) % 3)) cout << endl;		// three to a line
	}
	cout << endl << endl;

	while (1) {
		cout << "choose a digit index to modify or 'x' to exit: ";
		cin >> inval;

		if (inval.c_str() == "x") {
			return(-1);		// abort flag
		}

		digit = atoi(inval.c_str());		// convert input to integer

		if ((digit >= 0) && (digit < sppm1->NumDigits)) {
			break;
		}
		else {
			cout << "illegal digit index, try again" << endl;
		}
	}

	cout << "the digit index chosen is: " << digit << endl;
	cout << "the digit modulus chosen is: " << sppm1->Rn[digit]->GetPowMod2();
//	cout << "the value of this digit is currently: " << ppm1->Rn[digit]->Digit << endl;
	cout << endl;

	return(digit);

}

int get_digit_value(int dig_pos) {
PPM* ppm1 = new PPM(0);
string inval;
int i, digit;


	if (!((dig_pos >= 0) && (dig_pos < ppm1->NumDigits))) {
		cout << "ERROR: digit index out of range" << endl;
		return(-1);
	}

	while (1) {
		cout << "OK, now enter a new value for the digit: (0 --> " << ppm1->Rn[dig_pos]->GetPowMod2()-1 << ") : ";
		cin >> inval;
		digit = atoi(inval.c_str());		// convert input to integer

		if (!((digit >= 0) && (digit < ppm1->Rn[dig_pos]->GetPowMod2()))) {		// if not in range, then a problem
			cout << "digit value not in range, try again" << endl;
		}
		else {
			break;
		}
	}

	cout << endl;
	cout << "you entered a new digit value : " << digit << endl;
	cout << endl;

	return(digit);

}

// returns next modulus that is larger than small_modulus but less than all others
int get_next_modulus(int small_modulus) 
{
	int i;
	PPM ppm1;
	int dig_mod, small_mod;

	small_mod = 0;

	for(i = 0; i < ppm1.NumDigits; i++) {

		dig_mod = ppm1.Rn[i]->GetPowMod2();
		if (dig_mod > small_modulus) {
			if (!small_mod) {
				small_mod = dig_mod;
			}
			else if (dig_mod < small_mod) {
				small_mod = dig_mod;
			}
		}

	}

	return(small_mod);

}

// this routine generates the correction constants
// the routine assume the first P-2 digits are non-redundant, the last two digits are redundant
// this system supports single digit correction only
void generate_ecc_constants(ECC_t *ecc) 
{

	int i, j, mod_val;

	PPM *Mi = new PPM(0);			// dummy variable to access modulus value, and NumDigits
	PPM *two = new PPM(2);			// variable for dividing by two
	PPM *rem = new PPM(0);			// dummy variable, remainder for division routine
	
	int NumDigits = Mi->NumDigits;


	ecc->Mi_vect.resize(NumDigits);			// resize all vector elements of the ecc structure
	ecc->Mn_vect.resize(NumDigits);
	ecc->Ci_vect.resize(NumDigits);

	for (j = 0; j < NumDigits; j++) {		// generate the Mi vector values first
		ecc->Mi_vect[j].Assign(1);

		for (i = 0; i < NumDigits; i++) {

			if (j != i) {
				ecc->Mi_vect[j].Mult(Mi->Rn[i]->GetPowMod2());
			}

		}

		ecc->Ci_vect[j].Assign(&ecc->Mi_vect[j]);		// generate array of negative value correction constant
		ecc->Ci_vect[j].Complement();
	}

// upgraded this section to support digit modulus not in order
	mod_val = 0;
	ecc->M_half->Assign(1);
	for (i = 0; i < NumDigits - 2; i++) {		// compute total magnitude of number range M, multiply only non-redundant modulus  (first P-2 digits, p0<p1<p2<...)
		mod_val = get_next_modulus(mod_val);
//		cout << "modulus: " << mod_val << endl;
//		ecc->M_half->Mult(ecc->M_half->Rn[i]->GetPowMod2());
		ecc->M_half->Mult(mod_val);
	}

	ecc->Mt->Assign(ecc->M_half);									// save a copy of total magnitude of non-redundnat number range
	ecc->M_half->DivStd(two, rem);							// divide total range by two to get positive range which is only half M!

	for (i = 0; i < NumDigits; i++) {			// build an array of negative range check values for each trial digit set
		ecc->Mn_vect[i].Assign(&ecc->Mi_vect[i]);
		ecc->Mn_vect[i].Sub(ecc->M_half);
	}

}

// test routine that corrects a single digit error in a N+2 redundant residue format
// the routine handles singed values which are encoded using method of complements
// error correction constants are passed as a parameter in ecc
ERROR_CODES correct_digit(SPPM *val, ECC* ecc)
{
	int i, j;
	int pos_cnt = 0;				// number of posiive range checks that pass
	int neg_cnt = 0;				// number of negative range checks that pass

	SPPM *arg1 = new SPMF(0);			// signed temp variable in residue format
	SPPM *corrected = new SPPM(0);		// temporary store of the corrected value during error detection/correction

	PPM *Mi = new PPM(0);			// dummy variable to access modulus value
	PPM *two = new PPM(2);			// variable for dividing by two
	PPM *rem = new PPM(0);			// dummy variable, remainder for division routine

	int NumDigits = Mi->NumDigits;	// get the word size of the RNS system

//	TEST EACH DIGIT FOR ERROR, IF FOUND, THEN COMPUTE ERROR CORRECTION
//  TEST every digit to determine the type of the error correction: 1) NO_ERROR, 2) SINGLE_ERROR, 3) UNCORRECTED

	for (i = 0; i < NumDigits; i++) {

		arg1->Assign(val);				// RELOAD TEST VALUE EACH LOOP
		arg1->Rn[i]->SkipDigit();		// geneate a trial digit set in each loop by skipping each digit 
										// of the tested RNS word

		if (ecc->M_half->Compare_w_skips(arg1)) {				// For each trial, check for positive value recovered, compare against valid positive range
			if (!pos_cnt) {										// we don't need to correct every digit if one has already been corrected (for efficiency)
				arg1->PPM::Extend();								// base extend to recover the value, since the digit in error is skipped
				corrected->Assign(arg1);							// copy the corrected value
			}
			pos_cnt += 1;										// count how many positive corrections occurred to determine error type

		}
		else if (arg1->PPM::Compare_w_skips(&ecc->Mn_vect[i])) {		// Check for negative value recovered, compare against valid negative range
			if (!neg_cnt) {											//	we don't need to correct every digit if one has already been corrected (for efficiency)
				arg1->PPM::Extend();								// base extend to recover the skipped digit value, but it is still wrong for negative results
				arg1->PPM::Add(&ecc->Ci_vect[i]);					// add the correction constnat, which adjusts the base exended value
				corrected->Assign(arg1);							// copy the correted value
			}
			neg_cnt += 1;										// count the number of corected negative values

		}
	}

	// DETERMINE TYPE OF CORRECTION RESULT 1) NO_ERROR, 2) CORRECTED DIGIT, 3) UNCORRECTED RESULT

	if ((pos_cnt == 1) && (neg_cnt == 0)) {			// did a single positive error detect occur?
		val->Assign(corrected);
		return SINGLE_ERROR;
	}
	else if ((pos_cnt == 0) && (neg_cnt == 1)) {	// did a single negative error detect occur?
		val->Assign(corrected);
		return SINGLE_ERROR;
	}
	else if ((pos_cnt == NumDigits) || (neg_cnt == NumDigits)) {		// did an error etect occur for each digit?
		return NO_ERROR;
	}
	else {
		return UNCORRECTED;		// else the value is uncorrected
	}

// clean-up allocated variables

	delete ecc;
	delete arg1;			// signed temp variable in residue format
	delete corrected;		// temporary store of the corrected value during error detection/correction

	delete Mi;			// dummy variable to access modulus value
	delete two;			// variable for dividing by two
	delete rem;

}


// test routine that only corrects the digit in a N+2 redundant residue format
ERROR_CODES correct_digit2(SPPM *val)
{
	int i, j;
	int pos_cnt = 0;				// number of posiive range checks that pass
	int neg_cnt = 0;				// number of negative range checks that pass

	SPPM *arg1 = new SPMF(0);			// signed temp variable in residue format
	SPPM *corrected = new SPPM(0);		// temporary store of the corrected value during error detection/correction

	PPM *M_half = new PPM(0);		// range check for positive numbers  (M/2)
	PPM *Mt = new PPM(0);			// total range of correctable numbers  (M)
	PPM *Mi = new PPM(0);			// dummy variable to access modulus value
	PPM *two = new PPM(2);			// variable for dividing by two
	PPM *rem = new PPM(0);			// dummy variable, remainder for division routine

	int NumDigits = Mi->NumDigits;

// GENERATE THE NEEDED CONSTANTS FOR ERROR CORRECTION

	std::vector<PPM> Mi_vect;		// array of trial digit ranges, used to construct negative ranges and correction constants
	Mi_vect.resize(NumDigits);

	std::vector<PPM> Mn_vect;		// negative range values, one for each trial digit set
	Mn_vect.resize(NumDigits);

	std::vector<PPM> Ci_vect;		// negative value correctiion constants, one for each trial digit set
	Ci_vect.resize(NumDigits);

	for (j = 0; j < NumDigits; j++) {
		Mi_vect[j].Assign(1);

		for (i = 0; i < NumDigits; i++) {

			if (j != i) {
				Mi_vect[j].Mult(Mi->Rn[i]->GetPowMod2());
			}

		}

		Ci_vect[j].Assign(&Mi_vect[j]);		// making array of negative value correction constant
		Ci_vect[j].Complement();
	}


	M_half->Assign(1);
	for (i = 0; i < NumDigits - 2; i++) {		// compute total magnitude of number range M, multiply only non-redundnat modulus
		M_half->Mult(M_half->Rn[i]->GetPowMod2());
	}

	Mt->Assign(M_half);									// save a copy of total magnitude of non-redundnat number range
	M_half->DivStd(two, rem);							// divide total range by two to get positive range which is only half M!

	for (i = 0; i < NumDigits; i++) {			// build an array of negative range check values for each trial digit set
		Mn_vect[i].Assign(&Mi_vect[i]);
		Mn_vect[i].Sub(M_half);
	}

//	TEST EACH DIGIT FOR ERROR, IF FOUND, THEN COMPUTE ERROR CORRECTION

	for (i = 0; i < NumDigits; i++) {

		arg1->Assign(val);				// RELOAD TEST VALUE EACH LOOP
		arg1->Rn[i]->SkipDigit();		// skip each digit for each dual range test next

		if (M_half->Compare_w_skips(arg1)) {				// Check for positive value recovered, compare against valid positive range
			arg1->PPM::Extend();
			corrected->Assign(arg1);
//			printf("POSITIVE VALUE RECOVERED\n");
			pos_cnt += 1;

		}
		else if (arg1->PPM::Compare_w_skips(&Mn_vect[i])) {		// Check for negative value recovered, compare against valid negative range
			arg1->PPM::Extend();
			arg1->PPM::Add(&Ci_vect[i]);
			corrected->Assign(arg1);
//			printf("NEGATIVE VALUE RECOVERED\n");
			neg_cnt += 1;

		}
	}

// DETERMINE TYPE OF CORRECTION RESULT 1) NO_ERROR, 2) CORRECTED DIGIT, 3) UNCORRECTED RESULT

	if ((pos_cnt == 1) && (neg_cnt == 0)) {			// did a single positive error detect occur?
		val->Assign(corrected);
		return SINGLE_ERROR;
	}
	else if ((pos_cnt == 0) && (neg_cnt == 1)) {	// did a single negative error detect occur?
		val->Assign(corrected);
		return SINGLE_ERROR;
	}
	else if ((pos_cnt == NumDigits) || (neg_cnt == NumDigits)) {		// did an error etect occur for each digit?
		return NO_ERROR;
	}
	else  {
		return UNCORRECTED;		// else the value is uncorrected
	}

}

