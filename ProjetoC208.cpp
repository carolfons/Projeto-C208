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
uint16_t rInstr; // Registrador que guarda a instrução a ser executada.
uint16_t rOpCode; // Registrador que guarda o opcode da intrução a ser executada.
uint16_t rOper[3]; // Registradores que guardam os operandos da intrução a ser executada.
uint16_t rAux; // Registrador auxiliar.
uint16_t mem_prog[256] = { 0x2604, 0x28D3, 0x2102, 0x2203, 0x361, 0x226, 0x1718, 0x1226,
0x33AC, 0x3200 }; 
int16_t mem_dados[256] = { 0xF3, 0x1B, 0xDB, 0x1, 0x8 };
int16_t s[10]; // Registradores de uso geral.
bloco_cache_instr cache_instr[3]; // A cache de instruções tem três blocos.

void inicializa_cache_instr();
void buscar_instr(int16_t pc);
int16_t ula(int16_t rs, int16_t rt, int16_t operacao);
void uc();
void preencheTodaMemProg();

int main()
{
	setlocale(LC_ALL, "");
	mem_dados[0xD3] = 0xA1;
	cout << "O primeiro teste demonstrará o funcionamento de todas as instruções"
		<< " da arquitetura.\n";
	do
	{
		cout << "Tecle Enter para realizá-lo:";
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
	cout << "O segundo teste demonstrará o que acontece quando o pc aponta para "
		<< "o endereço 0xff (último endereço da memória de programa) e ocorre um"
		<<" cache miss. Atente para o que acontece durante a atualização da "
		<<"cache durante o último ciclo de busca, decodificação e execução. "
		<<"Este teste é demorado.\n";
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

// Marca todos os blocos da cache como inválidos.
void inicializa_cache_instr()
{
	for (int i = 0; i < 3; i++)
	{
		cache_instr[i].valido = 0;
	}
}

/* Busca a próxima instrução e a salva no registrador rInstr. A busca é feita
 primeiramente na cache. Se houver um cache miss, a busca é feita na memória de
 instruções e, depois, a cache é atualizada. */
void buscar_instr(int16_t pc)
{
	cout << "Buscando instrução na cache:\n";
	/* Busca a instrução em todos os blocos da cache. Se houver um cache hit,
	rInstr recebe a instrução encontrada e o processo de busca termina.*/
	for (int i = 0; i < 3; i++)
	{
		if (cache_instr[i].valido == 1 && cache_instr[i].tag == pc) // O bloco de cache é válido e a tag é igual ao endereço apontado pelo pc.
		{
			cout << "  Cache hit. Instrução encontrada no bloco " << i << endl;
			rInstr = cache_instr[i].dados;
			cout << "  rInstr = cache_instr[" << i << "] = 0x" << hex << rInstr << endl;
			cout << "Busca concluída.\n";
			return;
		}
	}
	cout << "  Cache miss.\n";
	cout << "  Buscando instrução na memória:\n";
	cout << "    Valor do pc = 0x" << hex << pc << "\n";
	rInstr = mem_prog[pc];
	cout << "    rInstr = mem_prog[pc] = 0x" << hex << rInstr << endl;
	cout << "Busca concluída.\n\n";
	cout << "Atualizando a cache:\n";
	rAux = pc;
	cout << "  rAux = pc = 0x" << hex << pc << endl;
	/* Coloca nos blocos 0, 1 e 2 da cache os dados contidos em mem_prog[pc],
	mem_prog[pc + 1] e mem_prog[pc + 2], respectivamente. Contudo, se pc + 1 for
	maior do que 255 (último endereço da memória de programa), os blocos 0 e 1
	da cache serão marcados como inválidos. Além disso, se pc + 1 for menor ou 
	igual a 255, mas pc + 2 for maior do que 255, o bloco 2 da cache será
	marcado como inválido.*/
	for (int i = 0; i < 3; i++)
	{
		cout << "\n  Atualizando o bloco " << i << ":\n";
		if (rAux < 256) // rAux não aponta para um endereço após o fim da memória de programa.
		{
			cout << "    Marcando o bloco como válido.\n";
			cache_instr[i].valido = 1;
			cache_instr[i].tag = rAux;
			cout << "    Tag = rAux = 0x" << hex << rAux << endl;
			cache_instr[i].dados = mem_prog[rAux];
			cout << "    Dados = mem_prog[rAux] = 0x" << hex << mem_prog[rAux] << endl;
			cout << "    Incrementando rAux:\n";
			cout << "      Colocando nas entradas da ULA o valor de rAux e o valor 0x1.\n";
			cout << "      Enviando à ULA o comando para somar (código 0x0).\n";
			cout << "      Colocando o valor na saída da ULA em rAux.\n";
			rAux = ula(rAux, 1, 0x0);
			cout << "      Resultado: rAux = 0x" << hex << rAux << endl;
			cout << "    Incrementação concluída.\n";
		}
		else
		{
			cout << "    rAux aponta para um endereço após o fim da memória de "
				<<"programa. Este bloco da cache será marcado como inválido.\n";
			cache_instr[i].valido = 0;
		}
		cout << "  Bloco atualizado.\n";
	}
	cout << "Atualização da cache concluída.\n";
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
	cout << "\nDecodificando instrução:\n";
	cout << "  Obtendo opcode:\n";
	rOpCode = rInstr >> 12; // Opcode
	cout << "    rOpCode = rInstr >> 12 = 0x" << hex << rOpCode << endl;
	if (rOpCode == 0x0) // Adicionar -> rd = rs + rt
	{
		cout << "  O opcode corresponde à instrução adicionar (add rd, rs, rt)."
			<< "\n\n";
		// Obtém rd (registrador de destino)
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
		cout << "    Resultado: rd é o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obtém rs (primeira parcela)
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
		cout << "    Resultado: rs é o registrador s" << rOper[1] << endl;
		cout << "  rs obtido.\n\n";

		// Obtém rt (segunda parcela)
		cout << "  Obtendo rt (segunda parcela):\n";
		rAux = rInstr >> 4;
		cout << "    rAux = rInstr >> 4 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[2] = rInstr & rAux;
		cout << "    rOper2 = rInstr & rAux = 0x" << hex << rOper[2] << endl;
		cout << "    Resultado: rt é o registrador s" << rOper[2] << endl;
		cout << "  rt obtido.\n";
		cout << "Instrução decodificada.\n\n";

		cout << "Executando instrução:\n";
		cout << "  Valor antigo de s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Valor de s" << dec << rOper[1] << " = 0x"
			<< hex << s[rOper[1]] << " Esta é a primeira parcela da soma.\n";
		cout << "  Valor de s" << dec << rOper[2] << " = 0x"
			<< hex << s[rOper[2]] << " Esta é a segunda parcela da soma.\n";
		cout << "  Colocando nas entradas da ULA os valores de s"
			<< dec << rOper[1] << " e s" << dec << rOper[2] << "." << endl;
		cout << "  Enviando à ULA o comando para somar (código 0x0).\n";
		cout << "  Colocando o valor na saída da ULA no registrador s" <<
			dec << rOper[0] << ".\n";
		s[rOper[0]] = ula(s[rOper[1]], s[rOper[2]], 0x0);
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex <<
			s[rOper[0]] << endl;
		cout << "Instrução executada.\n\n";
	}
	else if (rOpCode == 0x1) // Subtrair -> rd = rs - rt
	{
		cout << "  O opcode corresponde à instrução subtrair (sub rd, rs, rt)."
			<< "\n\n";
		// Obtém rd (registrador de destino)
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
		cout << "    Resultado: rd é o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obtém rs (minuendo)
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
		cout << "    Resultado: rs é o registrador s" << rOper[1] << endl;
		cout << "  rs obtido.\n\n";

		// Obtém rt (subtraendo)
		cout << "  Obtendo rt (subtraendo):\n";
		rAux = rInstr >> 4;
		cout << "    rAux = rInstr >> 4 = 0x" << hex << rAux << endl;
		rAux = rAux << 4;
		cout << "    rAux = rAux << 4 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[2] = rInstr & rAux; // rt = rOper[2]
		cout << "    rOper2 = rInstr & rAux = 0x" << hex << rOper[2] << endl;
		cout << "    Resultado: rt é o registrador s" << rOper[2] << endl;
		cout << "  rt obtido.\n";
		cout << "Instrução decodificada.\n\n";

		cout << "Executando instrução:\n";
		cout << "  Valor antigo de s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Valor de s" << dec << rOper[1] << " = 0x"
			<< hex << s[rOper[1]] << " Este é o minuendo.\n";
		cout << "  Valor de s" << dec << rOper[2] << " = 0x"
			<< hex << s[rOper[2]] << " Este é o subtraendo.\n";
		cout << "  Colocando nas entradas da ULA os valores de s"
			<< dec << rOper[1] << " e s" << dec << rOper[2] << "." << endl;
		cout << "  Enviando à ULA o comando para subtrair (código 0x1) s" 
			<< dec << rOper[2] << " de s" << dec << rOper[1] << "\n";
		cout << "  Colocando o valor na saída da ULA no registrador s" <<
			dec << rOper[0] << ".\n";
		s[rOper[0]] = ula(s[rOper[1]], s[rOper[2]], 0x1);
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex
			<< s[rOper[0]] << endl;
		cout << "Instrução executada.\n\n";
	}
	else if (rOpCode == 0x2) // Carregar palavra
	{
		cout << "  O opcode corresponde à instrução carregar palavra (lw rd, e)."
			<< "\n\n";
		// Obtém rd (registrador de destino)
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
		cout << "    Resultado: rd é o registrador s" << rOper[0] << endl;
		cout << "  rd obtido.\n\n";

		// Obtém e (endereço de memória)
		cout << "  Obtendo o endereço de memória que contém a palavra a ser "
			<< "carregada:\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 8;
		cout << "    rAux = rAux << 8 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr & rAux; // e = rOper[1]
		cout << "    rOper1 = rInstr & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: O endereço de memória é 0x" << hex << rOper[1] << endl ;
		cout << "  Endereço obtido.\n";
		cout << "Instrução decodificada.\n\n";

		cout << "Executando instrução:\n";
		cout << "  Valor antigo no registrador s" << dec << rOper[0]
			<< " = 0x" << hex << s[rOper[0]] << "\n";
		cout << "  Valor em mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "  Lendo a palavra no endereço 0x" << hex << rOper[1] << endl;
		cout << "  Salvando o valor obtido no registrador s" << dec << rOper[0]
			<< "\n";
		s[rOper[0]] = mem_dados[rOper[1]];
		cout << "  Resultado: s" << dec << rOper[0] << " = 0x" << hex
			<< s[rOper[0]] << endl;
		cout << "Instrução executada.\n\n";
	}
	else if (rOpCode == 0x3) // Salvar palavra
	{
		cout << "O opcode corresponde à instrução salvar palavra (sw rs, e)."
			<< "\n\n";
		// Obtém rs (registrador de origem)
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
		cout << "    Resultado: rs é o registrador s" << rOper[0] << endl;
		cout << "  rs obtido.\n\n";

		// Obtém e (endereço de memória)
		cout << "  Obtendo o endereço de memória no qual a palavra será salva:\n";
		rAux = rInstr >> 8;
		cout << "    rAux = rInstr >> 8 = 0x" << hex << rAux << endl;
		rAux = rAux << 8;
		cout << "    rAux = rAux << 8 = 0x" << hex << rAux << endl;
		rAux = ~rAux;
		cout << "    rAux = ~rAux = 0x" << hex << rAux << endl;
		rOper[1] = rInstr & rAux; // e = rOper[1]
		cout << "    rOper1 = rInstr & rAux = 0x" << hex << rOper[1] << endl;
		cout << "    Resultado: O endereço de memória é 0x" << hex << rOper[1] << endl;
		cout << "  Endereço obtido.\n";
		cout << "Instrução decodificada.\n\n";

		cout << "Executando instrução:\n";
		cout << "  Valor antigo em mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "  Valor no registrador s" << dec << rOper[0] << " = 0x"
			<< hex << s[rOper[0]] << endl;
		cout << "  Salvando o valor no registrador s" << dec << rOper[0] << " no"
			<< " endereço 0x" << hex << rOper[1] << endl;;
		mem_dados[rOper[1]] = s[rOper[0]];
		cout << "  Resultado: mem_dados[0x" << hex << rOper[1] << "] = 0x"
			<< hex << mem_dados[rOper[1]] << endl;
		cout << "Instrução executada.\n\n";
	}
	cout << "Incrementando o PC:\n";
	cout << "  Valor antigo do pc = 0x" << hex << pc << endl;
	cout << "  Colocando nas entradas da ULA o valor do PC e o valor 0x1.\n";
	cout << "  Enviando à ULA o comando para somar (código 0x0).\n";
	cout << "  Colocando o valor na saída da ULA no pc.\n";
	pc = ula(pc, 1, 0x0); // Incrementa o pc
	cout << "  Resultado: pc = 0x" << hex << pc << endl;
	cout << "Incrementação concluída.\n\n";
	cout << "----------------------------------------------------------------------\n\n";
}

/* Preenche toda a memória de programa com a instrução de código 0x2604 para
testar o comportamento especial da cache quando rAux aponta para um endereço
depois do fim da memória de programa.*/
void preencheTodaMemProg()
{
	for (int i = 0; i < 256; i++)
	{
		mem_prog[i] = 0x2604;
	}
}

