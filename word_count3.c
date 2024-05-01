//완성본 multi linked list
#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// User structure type definition
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tWord		*dataPtr;//단어 구조체 가리키는 포인터
	struct node	*link; // 단어순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;//메타데이터 - 노드의 개수 count
	NODE	*head; // 단어순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic( LIST *list, char *word);

// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc);

// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList); // 단어순
void print_dic_by_freq( LIST *pList); // 빈도순

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
	//string1-string2 (아스키 코드 기반, so 양수면 string1이 더 뒤
}
////////////////////////////////////////////////////////////////////////////////
// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	int ret = (int) p2->freq - p1->freq;
	
	if (ret != 0) return ret;
	
	return strcmp( p1->word, p2->word);
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	char word[1000];
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_WORD;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	//creates an empty list
	list = createList();
	
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}


	while(fscanf( fp, "%s", word) != EOF)
	{
		// 사전(단어순 리스트) 업데이트
		
		update_dic( list, word);
	}


	fclose( fp);

	if (option == SORT_BY_WORD) {
		
		// 단어순 리스트를 화면에 출력
		print_dic( list);
	}

	
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_dic_by_freq( list);
	}
	
	// 단어 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST* createList(void) {
	LIST* list = (LIST*)malloc(sizeof(LIST));
	if (list) {
		list->count = 0;
		list->head = NULL;
		list->head2 = NULL;
	}
	return list;
}


//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList(LIST* pList) {
	

	NODE* current = pList->head;
	while (current != NULL) {
		NODE* temp = current;
		current = current->link;
		destroyWord(temp->dataPtr); // 단어 구조체 메모리 해제
		free(temp);
		
	}
	free(pList);
	
}


// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search(LIST* pList, NODE** pPre, NODE** pLoc, tWord* pArgu) {
	*pPre = NULL;
	*pLoc = pList->head;
	while (*pLoc != NULL) {
		int cmp = compare_by_word((*pLoc)->dataPtr, pArgu);
		if (cmp >= 0) {
			if (cmp == 0) return 1; // Found
			break;
		}
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link;
	}
	return 0; // Not found
}


static int _search_by_freq(LIST* pList, NODE** pPre, NODE** pLoc, tWord* pArgu) {
	*pPre = NULL;
	*pLoc = pList->head2;
	while (*pLoc != NULL) {
		int cmp = compare_by_freq((*pLoc)->dataPtr, pArgu);
		if (cmp >= 0) { // 현재 노드의 빈도가 새 노드의 빈도보다 높거나 같으면, 삽입 위치 찾음
			if (cmp == 0) return 1;
			break;
		}
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link2;
	}
	return 0; // 명시적으로 찾지 못했음을 반환
}



// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert(LIST* pList, NODE* pPre, tWord* dataInPtr) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode) return 0; // Memory overflow

	newNode->dataPtr = dataInPtr;
	newNode->link = NULL;

	//새 노드를 첫 노드로 설정할 때
	if (pPre == NULL) { 
		newNode->link = pList->head;//헤드가 가리키는 노드
		pList->head = newNode;
	}
	//중간이나 끝에 넣을 때
	else { 
		newNode->link = pPre->link;
		pPre->link = newNode;
	}
	pList->count++;
	return 1; // Successful
}


// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
//과제 4의 createWord와 addNode 합쳐놧네
void update_dic(LIST* list, char* word) {

	tWord* newWord = createWord(word);
    NODE* pPre=NULL, * pLoc=NULL;

	int found = _search(list, &pPre, &pLoc, newWord);
	//addNode
    if (found) {
        pLoc->dataPtr->freq++; // 존재하는 단어의 빈도 증가
		destroyWord(newWord);
    }
    else {
		int insertResult = _insert(list, pPre, newWord);
		if (insertResult == 0) destroyWord(newWord);
    }
}



// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq(LIST* pList, NODE* pPre, NODE* pLoc) {
	if (pPre == NULL) { // Insert at beginning
		pLoc->link2 = pList->head2;
		pList->head2 = pLoc;
	}
	else { // Insert after pPre
		pLoc->link2 = pPre->link2;
		pPre->link2 = pLoc;
	}
}


// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency(LIST* list) {
	NODE* current = list->head;
	list->head2 = NULL; // Clear head2 before linking
	while (current != NULL) {
		NODE* pPre = NULL, * pLoc = NULL;
		_search_by_freq(list, &pPre, &pLoc, current->dataPtr);
		_link_by_freq(list, pPre, pLoc);
		current = current->link;
	}
}



// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic(LIST* pList) {
	NODE* pLoc = pList->head;
	while (pLoc != NULL) {
		printf("%s\t%d\n", pLoc->dataPtr->word, pLoc->dataPtr->freq);
		pLoc = pLoc->link;
	}
}//단어순

void print_dic_by_freq(LIST* pList) {
	NODE* pLoc = pList->head2;
	while (pLoc != NULL) {
		printf("%s\t%d\n", pLoc->dataPtr->word, pLoc->dataPtr->freq);
		pLoc = pLoc->link2;
	}
}//빈도순 


// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord* createWord(char* word) {
	tWord* newWord = (tWord*)malloc(sizeof(tWord));

	if (newWord) {
		newWord->word = strdup(word);
		newWord->freq = 1;
	}
	
	return newWord;
}


//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord(tWord* pWord) {
	if (pWord) {
		free(pWord->word); // 문자열 메모리 해제. freq는 int형이라 딱히 메모리 할당을 안시킴
		free(pWord);    // tWord 구조체 메모리 해제

	}
}

