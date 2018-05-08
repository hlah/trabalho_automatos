#ifndef _GLC_H_
#define _GLC_H_

#include <string>

class GLC {
	public:
		//// CONSTRUTOR ////
		GLC();
		// carrega GLC de arquivo
		bool carrega_arquivo(std::string arquivo_str);
		// salva GLC em arquivo
		bool salva_arquivo(std::string arquivo_str);
		// exibe GLC na tela
		bool exibe_arquivo(std::string arquivo_str);
		// simplifica o GLC
		bool simplifica();
		// converte para forma normal de Chomsky
		bool normaliza();
		// checa se gramática está simplificata
		bool simplificada();
		// checa se gramática está na forma normal de Chomsky
		bool normalizada();
	private:
};

#endif
