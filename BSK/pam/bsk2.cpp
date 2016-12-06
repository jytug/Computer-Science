#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
 
static struct pam_conv login_conv = {
	misc_conv,				/* przykładowa funkcja konwersacji z libpam_misc */
	NULL					/* ewentualne dane aplikacji (,,domknięcie'') */
};

void whitespace_all_nonalphas(std::string &str) {
	for (size_t i = 0; i < str.length(); i++)
		if (!isalpha(str[i]))
			str[i] = ' ';
}

void summarize_and_reset(std::unordered_map<std::string, int> &word_count) {
	std::vector<std::pair<int, std::string> > ord_summ;
	for (auto it = word_count.begin(); it != word_count.end(); it++) {
		std::pair<int, std::string> w(it->second, it->first);
		ord_summ.insert(ord_summ.begin(), w);		
	}
	std::sort(ord_summ.begin(), ord_summ.end());
	std::cout << "Podsumowanie: " << std::endl;
	size_t to_write = std::min(5, (int) ord_summ.size());
	for (size_t i = 0; i < to_write; ++i)
		std::cout << i << ". \"" << ord_summ[i].second
					<< "\" (" << ord_summ[i].first
					<< ") razy"
					<< std::endl;
	word_count.clear();
}

void popular_words() {
	std::unordered_map<std::string, int> word_count;
	std::string line;
	while (std::cin) {
		getline(std::cin, line);
		if (line == "==") {
			summarize_and_reset(word_count);
		} else if (line.find('.') != std::string::npos) {
			std::cerr << "Quitting the program" << std::endl;
			exit(0);
		} else {
			whitespace_all_nonalphas(line);
			std::istringstream iss(line);
			std::string word;
			while (iss) {
				iss >> word;
				word_count[word]++;
			}
		}
	}
}
 
int main() {
	pam_handle_t* pamh = NULL;
	int retval;
	char *username = NULL;
 
	retval = pam_start("Badania korpusowe?", username, &login_conv, &pamh);
	if (pamh == NULL || retval != PAM_SUCCESS) {
		fprintf(stderr, "Error when starting: %d\n", retval);
		return 1;
	}
 
	retval = pam_authenticate(pamh, 0);	/* próba autoryzacji */
	if (retval != PAM_SUCCESS) {
		fprintf(stderr, "Nieudane logowanie!\n");
		return 2;
	} else {
		fprintf(stderr, "Udane logowanie.\n");
	}
 
	printf("OK\n");	/* rzekomy kod aplikacji */
 
	pam_end(pamh, PAM_SUCCESS);
	return 0;
}
