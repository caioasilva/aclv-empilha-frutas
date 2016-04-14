#pragma once
template<class T>
class Pilha
{
public:
	Pilha(int tam = 4);
	Pilha(Pilha<T>& p);
	~Pilha();
	bool coloca(T item);
	T retira();
	int get_tam() const;
	int get_quant_itens() const;
	bool cheia() const;
	bool vazia() const;
	T get_item(int id) const;

private:
	int tamanho;
	T* pilha;
	int topo;
};

template<class T>
Pilha<T>::Pilha(int tam)
{
	if (tam>0)
	{
		tamanho = tam;
	}
	else
	{
		tamanho = 1;
	}
	pilha = new T[tamanho];
	topo = -1;
}
template<class T>
Pilha<T>::Pilha(Pilha<T>& p)
{
	tamanho = p.get_tam();
	topo = -1;
	pilha = new T[tamanho];
	int i;
	for (i = 0; i < tamanho; i++)
	{
		this->coloca(p.retira());
	}
}
template<class T>
Pilha<T>::~Pilha()
{
	delete[] pilha;
}
template<class T>
bool Pilha<T>::coloca(T item)
{
	if ((topo+1) >= tamanho)
		return false;

	pilha[++topo] = item;
	return true;
}
template<class T>
T Pilha<T>::retira()
{
	if (topo>-1)
		return pilha[topo--];
	else
		return T();
}
template<class T>
T Pilha<T>::get_item(int id) const
{
	if (id>=0&&id<=topo)
		return pilha[id];
	else
		return T();
}
template<class T>
int Pilha<T>::get_tam() const
{
	return tamanho;
}
template<class T>
int Pilha<T>::get_quant_itens() const
{
	return topo + 1;
}
template<class T>
bool Pilha<T>::cheia() const
{
	if (topo+1 >= tamanho)
		return true;
	else
		return false;
}
template<class T>
bool Pilha<T>::vazia() const
{
	if (topo==-1)
		return true;
	else
		return false;
}