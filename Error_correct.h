#pragma once

struct ECC {

	PPM *M_half = new PPM(0);		// range check for positive numbers  (M/2)
	PPM *Mt = new PPM(0);			// total range of correctable numbers  (M)

	std::vector<PPM> Mi_vect;
	std::vector<PPM> Mn_vect;
	std::vector<PPM> Ci_vect;

};

typedef ECC ECC_t;		// error correction constants type (structure of error correction constants)

enum ERROR_CODES { NO_ERROR, SINGLE_ERROR, UNCORRECTED };

void generate_ecc_constants(ECC_t* ecc);
ERROR_CODES correct_digit(SPPM *arg1);
ERROR_CODES correct_digit2(SPPM *arg1);

void correction_tests(void);