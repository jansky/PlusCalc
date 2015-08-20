typedef struct PCIFunction
{
	std::string name;
	std::vector<PCCalcToken> tokens;
	std::vector<char> variable_names;
} PCIFunction;

void replaceAll(std::string& str, const std::string& from, const std::string& to);
long double evaluateInput(std::string input, long double ans, bool exitonexception, bool passexceptionson = false);
void evaluateCmdInput(int argc, char **argv);
void exceptionresult(PCException err, bool exitonexception);

PCIFunction pci_create_function(std::string name, std::vector<PCCalcToken> tokens, std::vector<char> variable_names);
int pci_get_function_id_by_name(std::string name);
long double pci_evaluate_function(PCIFunction function, std::string args);

bool pci_save_vars_funcs_to_file(std::string fname);
long double pci_load_vars_funcs_from_file(std::string fname, long double ans,bool exitonexception=false);
void pci_report_trig_mode();
bool pci_file_exists(std::string fname);

std::vector<double> pci_synthetic_div(std::vector<double> coefficients, double divisor);

bool pci_is_prime(long double n);
