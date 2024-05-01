//완성본
//dynamic array(동적 배열)
#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, qsort
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 단어 구조체
typedef struct {
	char	*word;		// 단어-메모리 낭비 막을라구 캐릭터 포인터로
	int		freq;		// 빈도
} tWord;

// 사전(dictionary) 구조체
typedef struct {
	int		len;		// 배열에 저장된 단어의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 단어의 수)
	tWord	*data;		// 단어 구조체 배열에 대한 포인터
} tWordDic;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count( FILE *fp, tWordDic *dic);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( tWordDic *dic);

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic);

// qsort를 위한 비교 함수
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2);

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2);

////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 사전을 초기화 (빈 사전을 생성, 메모리 할당)
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tWordDic *create_dic(void)
{
	tWordDic *dic = (tWordDic *)malloc( sizeof(tWordDic));//tWordDic 구조체에 메모리 할당
	
	dic->len = 0;//생성된 단어 사전의 길이 0으로 초기화(단어가 아직 사전에 추가되지 않음)
	dic->capacity = 1000;//단어 사전 초기 용량 1000으로 설정
	dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));//실제 단어와 빈도를 저장하는 배열에 초기 용량에 맞게 메모리 할당

	return dic;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)//메인 함수에 전달되는 정보의 갯수/
{
	tWordDic *dic;//단어 사전 가리킬 포인터 선언
	int option;//프로그램 실행 옵션 저장 변수 선언
	FILE *fp;//파일 포인터 선언
	
	if (argc != 3)//제대로 된 인수 못받을 때 프로그램 종료
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}
	
	//프로그램 실행 옵션 확인, option 변수 설정(정렬 옵션)
	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_WORD;//단어별로 정렬
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;//빈도별로 정렬
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}
	
	// 사전 초기화
	dic = create_dic();

	// 입력 파일 열기
	if ((fp = fopen( argv[2], "r")) == NULL)//사용자로부터 받은 argv[2]로 파일 open
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	// 입력 파일로부터 단어와 빈도를 사전에 저장
	word_count( fp, dic);

	fclose( fp);//사용 용도 끝나면 바로 닫기

	// 정렬 (빈도 내림차순, 빈도가 같은 경우 단어순)
	if (option == SORT_BY_FREQ) {
		qsort( dic->data, dic->len, sizeof(tWord), compare_by_freq);
	}
		
	// 사전을 화면에 출력
	print_dic( dic);

	// 사전 메모리 해제
	destroy_dic( dic);
	
	return 0;
}



// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count(FILE* fp, tWordDic* dic) {
	char word[256];
	while (fscanf(fp, "%255s", word) == 1) {
		int found = 0;
		for (int i = 0; i < dic->len; i++) {
			if (strcmp(dic->data[i].word, word) == 0) {
				dic->data[i].freq++;
				found = 1;
				break;
			}
		}
		if (!found) {
			if (dic->len == dic->capacity) {
				dic->capacity += 1000;
				dic->data=(tWord*)realloc(dic->data, sizeof(tWord) * dic->capacity);
			}
			dic->data[dic->len].word = strdup(word);
			dic->data[dic->len].freq = 1;
			dic->len++;
		}
	}
}

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic(tWordDic* dic) {
	for (int i = 0; i < dic->len; i++) {
		printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq);
	}
}

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic* dic) {
	for (int i = 0; i < dic->len; i++) {
		free(dic->data[i].word);
	}
	free(dic->data);
	free(dic);
}

// qsort를 위한 비교 함수
// 정렬 기준 : 단어
int compare_by_word(const void* n1, const void* n2) {
	const tWord* word1 = (const tWord*)n1;
	const tWord* word2 = (const tWord*)n2;
	return strcmp(word1->word, word2->word);
}

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq(const void* n1, const void* n2) {
	const tWord* word1 = (const tWord*)n1;
	const tWord* word2 = (const tWord*)n2;
	if (word1->freq == word2->freq) {
		return strcmp(word1->word, word2->word);
	}
	return word2->freq - word1->freq;
}
