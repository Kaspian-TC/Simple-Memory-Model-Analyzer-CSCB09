char *concat_wrong(char *s1, char *s2) 
{
  char result[70];

  strcpy (result, s1);
  strcat (result, s2);

  return result;
}


char *concat(char *s1, char *s2)
{
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1);
  if (result == NULL) 
  {
      printf ("Error: malloc failed\n");
      exit(1);
  }

  strcpy (result, s1);
  strcat (result, s2);

  return result;
}