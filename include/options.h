#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <string>

class Options {
	public:
		// obtém opções
		inline bool simplificar() const { return _simplificar; };
		inline bool normalizar() const { return _normalizar; };
		inline bool verificar() const { return _verificar; };
		inline const std::string& glc_entrada() const { return _glc_entrada; };
		inline const std::string& glc_saida() const { return _glc_saida; };
		inline const std::string& string_entrada() const { return _string_entrada; };
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
