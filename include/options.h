#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <string>

class Options {
	public:
		// obtém opções
		bool simplificar() const;
		bool normalizar() const;
		bool verificar() const;
		const std::string& glc_entrada() const;
		const std::string& glc_saida() const;
		const std::string& string_entrada() const;
		// lê argumentos
		bool le_argumentos(int argc, char** argv);
	private:
		bool _simplificar = false;
		bool _normalizar = false;
		bool _verificar = false;
		std::string _glc_entrada;
		std::string _glc_saida;
		std::string _string_entrada;
		
};

#endif // _OPTIONS_H_
