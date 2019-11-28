#include <iostream>
#include <clocale>
using namespace std;

struct bloco_cache_instr
{
	bool valido; // Representa o bit de validade do bloco de cache.
	uint16_t tag;
	int16_t dados;
};

uint16_t pc; // Registrador program counter.
uint16_t rInstr; // Registrador que guarda a instru��o a ser executada.
uint16_t rOpCode; // Registrador que guarda o opcode da intru��o a ser executada.
uint16_t rOper[3]; // Registradores que guardam os operandos da intru��o a ser executada.
uint16_t rAux; // Registrador auxiliar.
uint16_t mem_prog[256] = { 0x2604, 0x28D3, 0x2102, 0x2203, 0x361, 0x226, 0x1718, 0x1226,
0x33AC, 0x3200 }; 
int16_t mem_dados[256] = { 0xF3, 0x1B, 0xDB, 0x1, 0x8 };
int16_t s[10]; // Registradores de uso geral.
bloco_cache_instr cache_instr[3]; // A cache de instru��es tem tr�s blocos.

void inicializa_cache_instr();
void buscar_instr(int16_t pc);
int16_t ula(int16_t rs, int16_t rt, int16_t operacao);
void uc();
void preencheTodaMemProg();

int main()
{
	setlocale(LC_ALL, "");
	mem_dados[0xD3] = 0xA1;
	cout << "O primeiro teste demonstrar� o funcionamento de todas as instru��es"
		<< " da arquitetura.\n";
	do
	{
		cout << "Tecle Enter para realiz�-lo:";
	} while (cin.get() != '\n');
	cout << "\n";
	pc = 0;
	cout << "pc = 0x" << hex << pc << endl << endl;
	cout << "----------------------------------------------------------------------\n\n";
	for (int i = 0; i < 10; i++)
	{
		uc();
	}
	cout << "Primeiro teste terminado.\n\n";
	cout << "O segundo teste demonstrar� o que acontece quando o pc aponta para "
		<< "o endere�o 0xff (�ltimo endere�o da mem�ria de programa) e ocorre um"
		<<" cache miss. Atente para o que acontece durante a atualiza��o da "
		<<"cache durante o �ltimo ciclo de busca, decodifica��o e execu��o. "
		<<"Este teste � demorado.\n";
	do
	{
		cout << "Tecle Enter para realizar o segundo teste:";
	} while (cin.get() != '\n');

	preencheTodaMemProg();
	pc = 0;
	cout << "pc = 0x" << hex << pc << endl << endl;
	cout << "----------------------------------------------------------------------\n\n";
	for (int i = 0; i < 256; i++)
	{
		uc();
	}
	cout << "Segundo teste terminado\n";
}

// Marca todos os blocos da cache como inv�lidos.
void inicializa_cache_instr()
{
	for (int i = 0; i < 3; i++)
	{
		cache_instr[i].valido = 0;
	}
}

/* Busca a pr�xima instru��o e a salva no registrador rInstr. A busca � feita
 primeiramente na cache. Se houver um cache miss, a busca � feita na mem�ria de
 instru��es e, depois, a cache � atualizada. */
void buscar_instr(int16_t pc)
{
	cout << "Buscando instru��o na cache:\n";
	/* Busca a instru��o em todos os blocos da cache. Se houver um cache hit,
	rInstr recebe a instru��o encontrada e o processo de busca termina.*/
	for (int i = 0; i < 3; i++)
	{
		if (cache_instr[i].valido == 1 && cache_instr[i].tag == pc) // O bloco de cache � v�lido e a tag � igual ao endere�o apontado pelo pc.
		{
			cout << "  Cache hit. Instru��o encontrada no bloco " << i << endl;
			rInstr = cache_instr[i].dados;
			cout << "  rInstr = cache_instr[" << i << "] = 0x" << hex << rInstr << endl;
			cout << "Busca conclu�da.\n";
			return;
		}
	}
	cout << "  Cache miss.\n";
	cout << "  Buscando instru��o na mem�ria:\n";
	cout << "    Valor do pc = 0x" << hex << pc << "\n";
	rInstr = mem_prog[pc];
	cout << "    rInstr = mem_prog[pc] = 0x" << hex << rInstr << endl;
	cout << "Busca conclu�da.\n\n";
	cout << "Atualizando a cache:\n";
	rAux = pc;
	cout << "  rAux = pc = 0x" << hex << pc << endl;
	/* Coloca nos blocos 0, 1 e 2 da cache os dados contidos em mem_prog[pc],
	mem_prog[pc + 1] e mem_prog[pc + 2], respectivamente. Contudo, se pc + 1 for
	maior do que 255 (�ltimo endere�o da mem�ria de programa), os blocos 0 e 1
	da cache ser�o marcados como inv�lidos. Al�m disso, se pc + 1 for menor ou 
	igual a 255, mas pc + 2 for maior do que 255, o bloco 2 da cache ser�
	marcado como inv�lido.*/
	for (int i = 0; i < 3; i++)
	{
		cout << "\n  Atualizando o bloco " << i << ":\n";
		if (rAux < 256) // rAux n�o aponta para um endere�o ap�s o fim da mem�ria de programa.
		{
			cout << "    Marcando o bloco como v�lido.\n";
			cache_instr[i].valido = 1;
			cache_instr[i].tag = rAux;
			cout << "    Tag = rAux = 0x" << hex << rAux << endl;
			cache_instr[i].dados = mem_prog[rAux];
			cout << "    Dados = mem_prog[rAux] = 0x" << hex << mem_prog[rAux] << endl;
			cout << "    Incrementando rAux:\n";
			cout << "      Colocando nas entradas da ULA o valor de rAux e o valor 0x1.\n";
			cout << "      Enviando � ULA o comando para somar (c�digo 0x0).\n";
			cout << "      Colocando o valor na sa�da da ULA em rAux.\n";
			rAux = ula(rAux, 1, 0x0);
			cout << "      Resultado: rAux = 0x" << hex << rAux << endl;
			cout << "    Incrementa��o conclu�da.\n";
		}
		else
		{
			cout << "    rAux aponta para um endere�o ap�s o fim da mem�ria de "
				<<"programa. Este bloco da cache ser� marcado como inv�lido.\n";
			cache_instr[i].valido = 0;
		}
		cout << "  Bloco atualizado.\n";
	}
	cout << "Atualiza��o da cache conclu�da.\n";
	return;
}

int16_t ula(int16_t rs, int16_t rt, int16_t operacao)
{
	if (operacao == 0x0)
	{
		return rs + rt;
	}
	else // operacao = 0x1
		return rs - rt;
}

void uc()
{
	setlocale(LC_ALL, "");

	buscar_instr(pc);
	cout << "\nDecodificando instru��o:\n";
	cout << "  Obtendo opcode:\n";
	rOpCode = rInstr >> 12; // Opcode
	cout << "    rOpCode = rInstr >> 12 = 0x" << hex << rOpCode << endl;
	if (rOpCode == 0x0) // Adicionar -> rd = rs + rt
	{
		cout << "  O opcode corresponde � instru��o adicionar (add rd, rs, rt)."
			<< "\n\n";
		// Obt�m rd (registrador de destino)
		cout << "  Obtendo rd (registrador de destino):\n";
		rAux = rOpCode;
		cout << "    rAux = rOpCode = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[0] = rInstr >> 8;
		cout << "    rOper0 = rInstr >> 8 = 0x" << hex << rOper[0] << endl;
		rOper[0] = rOper[0] & rAux;
		cout << "    rOper0 = rOper0 & rAux = 0x" << hex << rOper[0] << endl;
		cout << "    Resultado: rd � o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obt�m rs (primeira parcela)
		cout << "  Obtendo rs (primeira parcela):\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr >> 4;
		cout << "    rOper1 = rInstr >> 4 = 0x" << hex << rOper[1] << endl;
		rOper[1] = rOper[1] & rAux;
		cout << "    rOper1 = rOper1 & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: rs � o registrador s" << rOper[1] << endl;
		cout << "  rs obtido.\n\n";

		// Obt�m rt (segunda parcela)
		cout << "  Obtendo rt (segunda parcela):\n";
		rAux = rInstr >> 4;
		cout << "    rAux = rInstr >> 4 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[2] = rInstr & rAux;
		cout << "    rOper2 = rInstr & rAux = 0x" << hex << rOper[2] << endl;
		cout << "    Resultado: rt � o registrador s" << rOper[2] << endl;
		cout << "  rt obtido.\n";
		cout << "Instru��o decodificada.\n\n";

		cout << "Executando instru��o:\n";
		cout << "  Valor antigo de s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Valor de s" << dec << rOper[1] << " = 0x"
			<< hex << s[rOper[1]] << " Esta � a primeira parcela da soma.\n";
		cout << "  Valor de s" << dec << rOper[2] << " = 0x"
			<< hex << s[rOper[2]] << " Esta � a segunda parcela da soma.\n";
		cout << "  Colocando nas entradas da ULA os valores de s"
			<< dec << rOper[1] << " e s" << dec << rOper[2] << "." << endl;
		cout << "  Enviando � ULA o comando para somar (c�digo 0x0).\n";
		cout << "  Colocando o valor na sa�da da ULA no registrador s" <<
			dec << rOper[0] << ".\n";
		s[rOper[0]] = ula(s[rOper[1]], s[rOper[2]], 0x0);
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex <<
			s[rOper[0]] << endl;
		cout << "Instru��o executada.\n\n";
	}
	else if (rOpCode == 0x1) // Subtrair -> rd = rs - rt
	{
		cout << "  O opcode corresponde � instru��o subtrair (sub rd, rs, rt)."
			<< "\n\n";
		// Obt�m rd (registrador de destino)
		cout << "  Obtendo rd (registrador de destino):\n";
		rAux = rOpCode;
		cout << "    rAux = rOpCode = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[0] = rInstr >> 8;
		cout << "    rOper0 = rInstr >> 8 = 0x" << hex << rOper[0] << endl;
		rOper[0] = rOper[0] & rAux; // rd = rOper[0]
		cout << "    rOper0 = rOper0 & rAux = 0x" << hex << rOper[0] << endl;
		cout << "    Resultado: rd � o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obt�m rs (minuendo)
		cout << "  Obtendo rs (minuendo):\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr >> 4;
		cout << "    rOper1 = rInstr >> 4 = 0x" << hex << rOper[1] << endl;
		rOper[1] = rOper[1] & rAux; // rs = rOper[1]
		cout << "    rOper1 = rOper1 & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: rs � o registrador s" << rOper[1] << endl;
		cout << "  rs obtido.\n\n";

		// Obt�m rt (subtraendo)
		cout << "  Obtendo rt (subtraendo):\n";
		rAux = rInstr >> 4;
		cout << "    rAux = rInstr >> 4 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[2] = rInstr & rAux; // rt = rOper[2]
		cout << "    rOper2 = rInstr & rAux = 0x" << hex << rOper[2] << endl;
		cout << "    Resultado: rt � o registrador s" << rOper[2] << endl;
		cout << "  rt obtido.\n";
		cout << "Instru��o decodificada.\n\n";

		cout << "Executando instru��o:\n";
		cout << "  Valor antigo de s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Valor de s" << dec << rOper[1] << " = 0x"
			<< hex << s[rOper[1]] << " Este � o minuendo.\n";
		cout << "  Valor de s" << dec << rOper[2] << " = 0x"
			<< hex << s[rOper[2]] << " Este � o subtraendo.\n";
		cout << "  Colocando nas entradas da ULA os valores de s"
			<< dec << rOper[1] << " e s" << dec << rOper[2] << "." << endl;
		cout << "  Enviando � ULA o comando para subtrair (c�digo 0x1) s" 
			<< dec << rOper[2] << " de s" << dec << rOper[1] << "\n";
		cout << "  Colocando o valor na sa�da da ULA no registrador s" <<
			dec << rOper[0] << ".\n";
		s[rOper[0]] = ula(s[rOper[1]], s[rOper[2]], 0x1);
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex
			<< s[rOper[0]] << endl;
		cout << "Instru��o executada.\n\n";
	}
	else if (rOpCode == 0x2) // Carregar palavra
	{
		cout << "  O opcode corresponde � instru��o carregar palavra (lw rd, e)."
			<< "\n\n";
		// Obt�m rd (registrador de destino)
		cout << "  Obtendo rd (registrador de destino):\n";
		rAux = rOpCode;
		cout << "    rAux = rOpCode = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[0] = rInstr >> 8;
		cout << "    rOper0 = rInstr >> 8 = 0x" << hex << rOper[0] << endl;
		rOper[0] = rOper[0] & rAux; // rd = rOper[0]
		cout << "    rOper0 = rOper0 & rAux = 0x" << hex << rOper[0] << endl;
		cout << "    Resultado: rd � o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obt�m e (endere�o de mem�ria)
		cout << "  Obtendo o endere�o de mem�ria que cont�m a palavra a ser "
			<< "carregada:\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 8;
		cout << "    rAux = rAux << 8 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr & rAux; // e = rOper[1]
		cout << "    rOper1 = rInstr & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: O endere�o de mem�ria � 0x" << hex << rOper[1] << endl ;
		cout << "  Endere�o obtido.\n";
		cout << "Instru��o decodificada.\n\n";

		cout << "Executando instru��o:\n";
		cout << "  Valor antigo no registrador s" << dec << rOper[0]
			<< " = 0x" << hex << s[rOper[0]] << "\n";
		cout << "  Valor em mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "  Lendo a palavra no endere�o 0x" << hex << rOper[1] << endl;
		cout << "  Salvando o valor obtido no registrador s" << dec << rOper[0]
			<< "\n";
		s[rOper[0]] = mem_dados[rOper[1]];
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex
			<< s[rOper[0]] << endl;
		cout << "Instru��o executada.\n\n";
	}
	else if (rOpCode == 0x3) // Salvar palavra
	{
		cout << "O opcode corresponde � instru��o salvar palavra (sw rs, e)."
			<< "\n\n";
		// Obt�m rs (registrador de origem)
		cout << "  Obtendo rs (registrador de origem):\n";
		rAux = rOpCode;
		cout << "    rAux = rOpCode = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux ~rAux = 0x" << hex << rAux << endl;
		rOper[0] = rInstr >> 8;
		cout << "    rOper0 = rInstr >> 8 = 0x" << hex << rOper[0] << endl;
		rOper[0] = rOper[0] & rAux; // rd = rOper[0]
		cout << "    rOper0 = rOper0 & rAux = 0x" << hex << rOper[0] << endl;
		cout << "    Resultado: rs � o registrador s" << rOper[0] << endl;
		cout << "  rs obtido.\n\n";

		// Obt�m e (endere�o de mem�ria)
		cout << "  Obtendo o endere�o de mem�ria no qual a palavra ser� salva:\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 8;
		cout << "    rAux = rAux << 8 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr & rAux; // e = rOper[1]
		cout << "    rOper1 = rInstr & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: O endere�o de mem�ria � 0x" << hex << rOper[1] << endl;
		cout << "  Endere�o obtido.\n";
		cout << "Instru��o decodificada.\n\n";

		cout << "Executando instru��o:\n";
		cout << "  Valor antigo em mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "  Valor no registrador s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Salvando o valor no registrador s" << dec << rOper[0] << " no"
			<< " endere�o 0x" << hex << rOper[1] << endl;;
		mem_dados[rOper[1]] = s[rOper[0]];
		cout << "  Resultado: mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "Instru��o executada.\n\n";
	}
	cout << "Incrementando o PC:\n";
	cout << "  Valor antigo do pc = 0x" << hex << pc << endl;
	cout << "  Colocando nas entradas da ULA o valor do PC e o valor 0x1.\n";
	cout << "  Enviando � ULA o comando para somar (c�digo 0x0).\n";
	cout << "  Colocando o valor na sa�da da ULA no pc.\n";
	pc = ula(pc, 1, 0x0); // Incrementa o pc
	cout << "  Resultado: pc = 0x" << hex << pc << endl;
	cout << "Incrementa��o conclu�da.\n\n";
	cout << "----------------------------------------------------------------------\n\n";
}

/* Preenche toda a mem�ria de programa com a instru��o de c�digo 0x2604 para
testar o comportamento especial da cache quando rAux aponta para um endere�o
depois do fim da mem�ria de programa.*/
void preencheTodaMemProg()
{
	for (int i = 0; i < 256; i++)
	{
		mem_prog[i] = 0x2604;
	}
}

