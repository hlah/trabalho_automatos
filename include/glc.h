#ifndef _GLC_H_
#define _GLC_H_

#include <string>
#include <map>
#include <vector>

class GLC {
	public:
		// carrega GLC de arquivo
		bool carrega_arquivo(std::string arquivo_str);
		// salva GLC em arquivo
		bool salva_arquivo(std::string arquivo_str);
		// exibe GLC na tela
		bool exibe();
		// simplifica o GLC
		bool simplifica();
		// converte para forma normal de Chomsky
		bool normaliza();
		// checa se possou alguma gramática aberta
		inline bool aberto() { return _aberto; };
		// checa se gramática está simplificata
		bool simplificada();
		// checa se gramática está na forma normal de Chomsky
		bool normalizada();
	private:
		// enumerador para o estado de leitura
		enum Estado {
			INICIO,
			TERMINAIS,
			VARIAVEIS,
			INICIAL,
			REGRAS,
		};
		// contador de terminais e variaveis
		int _term_contador = 0;
		int _vars_contador = 0;
		// mapas
		std::map<int,std::string> _int_para_term;
		std::map<std::string,int> _term_para_int;
		std::map<int,std::string> _int_para_var;
		std::map<std::string,int> _var_para_int;
		// regras
		std::map<int,std::vector<std::vector<int>>> _regras;
		// símbolo vazio
		const std::string _vazio = "V";
		// booleanos
		bool _aberto = false;
};

#endif
