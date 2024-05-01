//완성본
//doubly linked list
#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1 //Q:실행 종료 (프로그램 종료 전에 리스트를 위해 할당한 메모리를 모두 해제해야 함)
#define FORWARD_PRINT	2 //P:화면에 (순방향으로) list를 출력한다. (단어(word)와 빈도(freq)를 탭(tab)문자로 구분하여 출력)
#define BACKWARD_PRINT	3 //B:화면에 역방향으로 list를 출력한다. 
#define SEARCH			4 //S:사용자가 입력한 단어를 리스트에서 찾는다.
#define DELETE			5 //D:사용자가 입력한 단어를 리스트에서 삭제한다.
#define COUNT			6 //C:리스트에 포함된 단어(노드)의 수를 출력한다.

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
	tWord		*dataPtr;
	struct node	*llink; // backward pointer
	struct node	*rlink; // forward pointer
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);

// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가) -효율 위해서 그냥 이 함수에서 처리하기로 함
int addNode( LIST *pList, tWord *dataInPtr);

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr);

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr);

// returns number of nodes in list
//C키 눌렀을 때
int countList( LIST *pList);

// returns	1 empty
//			0 list has data
//이 프로그램에서는 안쓰여서 뭐 안만들어도 됨
int emptyList( LIST *pList);

// traverses data from list (forward)
//callback함수 인자로 넣어서 어떤 명령 수행할 지
void traverseList( LIST *pList, void (*callback)(const tWord *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tWord *));


// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return	word structure pointer
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);//대문자로 다 바꿈
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_word(const tWord *dataPtr)
{
	printf( "%s\t%d\n", dataPtr->word, dataPtr->freq);
}

// gets user's input
void input_word(char *word)
{
	fprintf( stderr, "Input a word to find: ");
	fscanf( stdin, "%s", word);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char word[100];
	tWord *pWord;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		pWord = createWord( word);
		
		// 이미 저장된 단어는 빈도 증가
		ret = addNode( list, pWord);
		
		if (ret == 0 || ret == 2) // failure or duplicated 안됐거나 이미 있음
		{
			destroyWord( pWord);
		}
	}
	
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tWord *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_word);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_word);
				break;
			
			case SEARCH:
				input_word(word);
				
				pWord = createWord( word);

				if (searchNode( list, pWord, &ptr)) print_word( ptr);
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
				
			case DELETE:
				input_word(word);
				
				pWord = createWord( word);

				if (removeNode( list, pWord, &ptr))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ptr->word, ptr->freq);
					destroyWord( ptr);
				}
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
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
		list->rear = NULL;
	}
	return list;
}


//  단어 리스트에 할당된 메모리를 돌면서 해제 (head node, data node, word data)
void destroyList(LIST* pList) {
	NODE* pLoc = pList->head;
	while (pLoc != NULL) {
		NODE* temp = pLoc;
		pLoc = pLoc->rlink;
		destroyWord(temp->dataPtr);//단어 구조체 해제
		free(temp);//노드 해제
		
		
	}
	free(pList);//헤드 해제-리스트 해제 완료
}


// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가)
//메모리 해제는 다 함수 밖에서 하도록
int addNode(LIST* pList, tWord* dataInPtr) {
	NODE* pPre = NULL, * pLoc = NULL;
	int found = _search(pList, &pPre, &pLoc, dataInPtr);

	// 중복된 키를 찾은 경우
	if (found) {
		pLoc->dataPtr->freq++; // 빈도 증가
		return 2; // 중복된 키 반환, 이후 해제 처리 메인에서
	}
	//새로운 노드 추가
	else {
		int insertResult = _insert(pList, pPre, dataInPtr);
		return insertResult;
	}
}




// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode(LIST* pList, tWord* keyPtr, tWord** dataOutPtr) {
	NODE* pPre, * pLoc;
	int found = _search(pList, &pPre, &pLoc, keyPtr);
	if (found) {
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;
	}
	return 0;
}


// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode(LIST* pList, tWord* pArgu, tWord** dataOutPtr) {
	NODE* pPre, * pLoc;
	int found = _search(pList, &pPre, &pLoc, pArgu);
	if (found) {
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}
	return 0;
}


// returns number of nodes in list
int countList(LIST* pList) {
	return pList->count;
}


// returns	1 empty
//			0 list has data
int emptyList(LIST* pList) {
	return (pList->count == 0);
}


// traverses data from list (forward)
//callback 함수 - 함수 포인터가 매개변수로 들어가는 함수
//콜백 함수 인자로 넣어서 어떤 명령 실행할 지
//ex traverseList(list, print_word) 뭐 이런 식
void traverseList(LIST* pList, void (*callback)(const tWord*)) {
	NODE* pLoc = pList->head;
	while (pLoc != NULL) {
		callback(pLoc->dataPtr);
		pLoc = pLoc->rlink;
	}
}


// traverses data from list (backward)
void traverseListR(LIST* pList, void (*callback)(const tWord*)) {
	NODE* pLoc = pList->rear;
	while (pLoc != NULL) {
		callback(pLoc->dataPtr);
		pLoc = pLoc->llink;
	}
}


// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert(LIST* pList, NODE* pPre, tWord* dataInPtr) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode) {
		// 메모리 할당 실패
		return 0;
	}

	newNode->dataPtr = dataInPtr; // 데이터 포인터 설정
	newNode->llink = newNode->rlink = NULL;

	// 새 노드를 첫 노드로 설정할 때(pPre가 없을 때)
	if (pPre==NULL) {
		//리스트가 아예 비어있음
		if (pList->head == NULL) {
			pList->head = pList->rear = newNode;
		}
		else {
			newNode->rlink = pList->head;
			pList->head->llink = newNode;
			pList->head = newNode;
		}
	}
	//새 노드를 중간이나 끝에 삽입할 때
	else {
		//우선 newNode 양 측에 연결
		newNode->rlink = pPre->rlink;
		newNode->llink = pPre;

		// 후행자를 newNode와 연결하기
		if (pPre->rlink == NULL) pList->rear = newNode;//끝에 삽입
		else pPre->rlink->llink=newNode;//중간 삽입
		pPre->rlink = newNode;
	}

	pList->count++;
	return 1;
}


// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete(LIST* pList, NODE* pPre, NODE* pLoc, tWord** dataOutPtr) {
	*dataOutPtr = pLoc->dataPtr; // 삭제할 데이터 반환

	if (pLoc == pList->head) { // 시작 노드 삭제
		pList->head = pLoc->rlink;
		if (pList->head == NULL) pList->rear = NULL;
		else pLoc->rlink->llink = NULL;
	}
	else if (pLoc == pList->rear) { // 끝 노드 삭제
		pPre->rlink = NULL;
		pList->rear = pLoc->llink;
	}
	else { // 중간 노드 삭제
		pPre->rlink = pLoc->rlink;
		pLoc->rlink->llink = pPre;
		//pLoc->llink->rlink = pLoc->rlink; pPre 사실 굳이 필요 없긴 한데ㅎ
		//pLoc->rlink->llink = pLoc->llink;
	}


	free(pLoc);//단어 삭제한거니 이제 노드 더이상 필요 없음.
	//이후 단어 구조체는 main에서 dataOut 넣어 destroyWord로 삭제 
	pList->count--;
}


// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search(LIST* pList, NODE** pPre, NODE** pLoc, tWord* pArgu) {
	*pPre = NULL;
	*pLoc = pList->head;

	while (*pLoc != NULL) {
		int cmp = compare_by_word((*pLoc)->dataPtr, pArgu);
		if (cmp >= 0) {
			if (cmp == 0) return 1; // 찾음
			else break; // 찾는 값이 더 큼
		}
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	return 0; // 못 찾음
}


////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return	word structure pointer
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
void destroyWord(tWord* pNode) {
	free(pNode->word); // 단어 데이터 해제
	free(pNode); // 구조체 자체 해제

}
