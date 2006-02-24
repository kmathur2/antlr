
#define	ANTLR3_MEM_DEBUG

#include    <antlr3.h>

void myfree(void * p)
{
    free(p);
}

#define	TEST_HASH_COUNT	10000
#define	TEST_HASH_SIZE	3011

int main()
{
    pANTLR3_INPUT_STREAM input;
    int	i;
    ANTLR3_UINT8	    key[256];
    pANTLR3_HASH_TABLE	ht;

    pANTLR3_HASH_ENUM	en;

    ANTLR3_UINT32	c;

    pANTLR3_STRING_FACTORY  sf;
    pANTLR3_STRING	    string;

    pANTLR3_TOKEN_FACTORY   tf;
    pANTLR3_COMMON_TOKEN    tok;

    unsigned char   * retkey;
    void	    * retdata;

    input   = antlr3AsciiFileStreamNew("C:/iscsrc/users/5.1.mv/modules/Antlr/mvindex/src/mvindexcommands.g");

    while   ((c = input->LA(input, 1)) != ANTLR3_CHARSTREAM_EOF)
    {
	input->consume(input);
	printf("%c", c);
    }


    ht	= antlr3HashTableNew(TEST_HASH_SIZE);

    if	(ht >= 0)
    {
	for (i=0; i<TEST_HASH_COUNT; i++)
	{
	    sprintf(key, "%d", i);
	    ht->put(ht, key, (void *)strdup(key), myfree);
	    if	((i % (TEST_HASH_COUNT/42)) == 0)
	    {
		printf("Key added: %s\n", key);
	    }
	}

	en  = antlr3EnumNew(ht);

	if  (en != NULL)
	{
	    i	= 0;
	    /* Enumerate all entries */
	    while   (en->next(en, &retkey, &retdata) == ANTLR3_SUCCESS)
	    {
		if  ((i % (TEST_HASH_COUNT/42)) == 0)
		{
		    printf("Enumerated key %s\n", retkey);
		}
		i++;
	    }
	}

	en->free(en);

	for (i=0; i<TEST_HASH_COUNT; i++)
	{
	    sprintf(key, "%d", i);
	    retkey = ht->get(ht, key);

	    if	((i % (TEST_HASH_COUNT/42)) == 0)
	    {
		printf("Key found: %s\n", retkey);
	    }
	}

	for (i=TEST_HASH_COUNT-1; i >= 0; i--)
	{
	    sprintf(key, "%d", i);
	    ht->del(ht, key);
	    if	((i % (TEST_HASH_COUNT/42)) == 0)
	    {
		printf("Key deleted: %s\n", key);
	    }
	}
	
	for (i=0; i<TEST_HASH_COUNT; i++)
	{
	    sprintf(key, "%d", i);
	    ht->put(ht, key, (void *)strdup(key), myfree);
	    if	((i % (TEST_HASH_COUNT/42)) == 0)
	    {
		printf("Key added: %s\n", key);
	    }
	}

	ht->free(ht);
    }

    sf	= antlr3StringFactoryNew();

    /* Let's create some strings
     */
    for (i=0; i<TEST_HASH_COUNT; i++)
    {
	sprintf(key, "This is string %d", i);
	string	= sf->newPtr(sf, (pANTLR3_UINT8)key, (ANTLR3_UINT32)strlen(key));
    }

    /* Now we can try some string manipulation
     */
    for	(i=0; i<100; i++)
    {
	string->addi(string, i);
    }

    for (i=0 ; i<100; i++)
    {
	string->insert(string, i, "->ins<-");
    }
    
    string->append(string, "\n\n    \t\t   \r\r   ");

    string->factory->destroy(string->factory, string);

    /* Close the factory, should release all the strings and tables and everything
     */
    sf->close(sf);

    tf	    = input->tokFactory;

    /* Create loads and loads of tokens
     */
    for	(i=0; i<TEST_HASH_COUNT; i++)
    {
	tok = tf->newToken(tf);
	if	((i % (TEST_HASH_COUNT/42)) == 0)
	{
	    /* Deliberately throws away the pointer returned by toString()
	     * to make sure the string factory cleans itself up.
	     */
	    printf("%s\n", tok->toString(tok)->text);
	}
    }
    

    ANTLR3_MEM_REPORT(ANTLR3_FALSE);
    input->close(input);
    ANTLR3_MEM_REPORT(ANTLR3_TRUE);

    return 0;
}