/*对生成的出口临时文件进行处理,以及出口处理*/

/*文件分解*/
int spli_file(RuntimeInfo *runtime_info,StatInfo* stat_info)
{
    char tmp_path[512];
    char tmp_pathk[512];
    char tmp_pathq[512];
    char tmp_pathi[512];
    char tmp_pathd[512];
    char tmp_pathg[512];
    char tmp_pathig[512];
	char tmp_pathpps[512];
    char dealed_bill[1024];
    int len=0;
    stat_info->outd=0;
    stat_info->outk=0;
    stat_info->outq=0;
    stat_info->outi=0;
    stat_info->outg=0;
    stat_info->outig=0;
	stat_info->outpps=0;
    FILE* fp=NULL;
    FILE* fpk=NULL;
    FILE* fpq=NULL;
    FILE* fpd=NULL;
    FILE* fpi=NULL;
    FILE* fpg=NULL;
    FILE* fpig=NULL;
	FILE* fpps=NULL;
    sprintf(tmp_path, "%s/%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    fp=fopen(tmp_path,"rb");
    if(fp==NULL)
    return 0;
    sprintf(tmp_pathk, "%s/k%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    fpk=fopen(tmp_pathk,"wb");
    if(fpk==NULL)
    return -1;
    sprintf(tmp_pathq, "%s/q%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    fpq=fopen(tmp_pathq,"wb");
    if(fpq==NULL)
    return -1;
    sprintf(tmp_pathd, "%s/d%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    fpd=fopen(tmp_pathd,"wb");
    if(fpd==NULL)
    return -1;
    sprintf(tmp_pathi, "%s/i%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    fpi=fopen(tmp_pathi,"wb");
    if(fpi==NULL)
    return -1;
    sprintf(tmp_pathg, "%s/g%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    fpg=fopen(tmp_pathg,"wb");
    if(fpg==NULL)
    return -1;
    sprintf(tmp_pathig, "%s/ig%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    fpig=fopen(tmp_pathig,"wb");
    if(fpig==NULL)
    return -1;
	sprintf(tmp_pathpps, "%s/pps%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
	fpps=fopen(tmp_pathpps,"wb");
	if(fpps==NULL)
    return -1;
     
	 
    while(!feof(fp))
    {
	bzero(dealed_bill,sizeof(dealed_bill));
	fgets(dealed_bill,1000,fp);
    len=strlen(dealed_bill);
    if(len<1)
    continue;
	if(!memcmp(dealed_bill,"vc",2) && !memcmp(dealed_bill+187,"5",1))
	   {
		if(fwrite(&dealed_bill,len,1,fpk)!=1)
	     return -1;
	     stat_info->outk++;
		 continue;
	   }
    else if(!memcmp(dealed_bill,"vc",2) &&(!memcmp(dealed_bill+193,"035",3) || !memcmp(dealed_bill+193,"032",3) || !memcmp(dealed_bill+193,"033",3)))
	   {
	   if(fwrite(&dealed_bill,len,1,fpq)!=1)
	     return -1;
	     stat_info->outq++;
		 continue;
	   }
	else if(!memcmp(dealed_bill,"vc",2) && !memcmp(dealed_bill+187,"1",1))
	   {
	   if(fwrite(&dealed_bill,len,1,fpps)!=1)
		  return -1;
		  stat_info->outpps++;
		  continue;
	   }

	else if((!memcmp(dealed_bill,"vc",2)&&!memcmp(dealed_bill+188,"3",1)) || !memcmp(dealed_bill,"vf",2))
	   {
	   if(fwrite(&dealed_bill,len,1,fpd)!=1)
	     return -1;
	     stat_info->outd++;
		 continue;
	   }
	else if(!memcmp(dealed_bill,"vc",2)&&!memcmp(dealed_bill+188,"5",1))
	   {
	   if(fwrite(&dealed_bill,len,1,fpi)!=1)
	     return -1;
	     stat_info->outi++;
		 continue;
	   }
	else if(!memcmp(dealed_bill,"gg",2)&&!memcmp(dealed_bill+408,"3",1))
	   {
	   if(fwrite(&dealed_bill,len,1,fpg)!=1)
	     return -1;
	     stat_info->outg++;
		 continue;
	   }
	  
	else if(!memcmp(dealed_bill,"gg",2) && !memcmp(dealed_bill+408,"5",1))
	   {
	    if(fwrite(&dealed_bill,len,1,fpig)!=1)
	     return -1;
	     stat_info->outig++;
		 continue;
	   }
	   else continue;
      }
      fclose(fp);
      fclose(fpk);
      fclose(fpq);
      fclose(fpd);
      fclose(fpi);
      fclose(fpg);
      fclose(fpig);
	  fclose(fpps);
      if(stat_info->outk<1)
      {
      	remove(tmp_pathk);
      }
       if(stat_info->outq<1)
      {
      	remove(tmp_pathq);
      }
       if(stat_info->outd<1)
      {
      	remove(tmp_pathd);
      }
       if(stat_info->outi<1)
      {
      	remove(tmp_pathi);
      }
       if(stat_info->outg<1)
      {
      	remove(tmp_pathg);
      }
      if(stat_info->outig<1)
      {
      	remove(tmp_pathig);
      }
	  if(stat_info->outpps<1)
	  {
	  remove(tmp_pathpps);
	  }
      remove(tmp_path);
      (stat_info->out[get_stat_index(stat_info->file_name)])=0;
      return 0;
}

int move(RuntimeInfo *runtime_info,StatInfo* stat_info)
{
  int i,flag=1;
  char tmp_path[512];
  char des_path[512];
  sprintf(tmp_path, "%s/%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    for(i = 0; i< 6; i++)
    {
        if(stat_info->out[i])
        {
            flag = 0;
			if(!memcmp(stat_info->file_name+VPMNINFILE,VPMNFILESTR,VPMNSTRLEN))
			sprintf(des_path, "%s/pvpmn/p%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+1);
	                else if(!memcmp(stat_info->file_name+3,"to",2))
	                sprintf(des_path, "%s/pvct/pvct%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
	                else if(!memcmp(stat_info->file_name+3,"ti",2))
	                sprintf(des_path, "%s/pvco/pvco%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
	                else 
	                sprintf(des_path, "%s/pvcr/pvcr%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
                        if(move_file(tmp_path, des_path))
                        return 1;
        }
    } 
    if(flag)
    remove(tmp_path);
    
    sprintf(tmp_path, "%s/ig%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    if(stat_info->outig)
     {
     	 sprintf(des_path, "%s/pggi/pggi%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);
     
     sprintf(tmp_path, "%s/k%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    if(stat_info->outk)
     {
     	 sprintf(des_path, "%s/pvcm/pvcm%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);

     sprintf(tmp_path, "%s/d%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    if(stat_info->outd)
     {
     	 sprintf(des_path, "%s/pvcr/pvcr%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);

     sprintf(tmp_path, "%s/i%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    if(stat_info->outi)
     {
     	 sprintf(des_path, "%s/pvci/pvci%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);

     sprintf(tmp_path, "%s/q%s.out", (runtime_info->parameter_info)->tmp_dir, stat_info->file_name);
    if(stat_info->outq)
     {
     	 sprintf(des_path, "%s/pvch/pvch%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);

     sprintf(tmp_path, "%s/g%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
    if(stat_info->outg)
     {
     	 sprintf(des_path, "%s/pggr/pggr%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
     	 if(move_file(tmp_path, des_path))
               return 1;
     }
     else
     remove(tmp_path);

	 sprintf(tmp_path, "%s/pps%s.out", (runtime_info->parameter_info)->tmp_dir,stat_info->file_name);
     if(stat_info->outpps)
	 {
	     sprintf(des_path, "%s/pps/pps%s", (runtime_info->parameter_info)->out_dir, stat_info->file_name+3);
		 if(move_file(tmp_path, des_path))
             return 1;
	 }
	 else
	 remove(tmp_path);

	 (stat_info->out[get_stat_index(stat_info->file_name)])=(stat_info->out[get_stat_index(stat_info->file_name)])+stat_info->outd+stat_info->outk+stat_info->outq+stat_info->outi+stat_info->outg+stat_info->outig+stat_info->outpps;
	 return 0;

}
int resume_file_tmp(RuntimeInfo *runtime_info,char *file_name)
{
	 char tmp_path[512];
	 char path[512];
	 sprintf(tmp_path, "%s/d%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
              if(!is_file_ok(tmp_path))
               {
               sprintf(path, "%s/runlog/pd%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
               if(move_file(tmp_path, path))
               return -1;
              }
      sprintf(tmp_path, "%s/i%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
            if(!is_file_ok(tmp_path))
            {
                sprintf(path, "%s/runlog/pi%s", (runtime_info->parameter_info)->log_dir,file_name + 1);
                if(move_file(tmp_path, path))
                    return -1;
            }
      sprintf(tmp_path, "%s/k%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
            if(!is_file_ok(tmp_path))
            {
                sprintf(path, "%s/runlog/pk%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
                if(move_file(tmp_path, path))
                    return -1;
            }
      sprintf(tmp_path, "%s/q%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
            if(!is_file_ok(tmp_path))
            {
                sprintf(path, "%s/runlog/pq%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
                if(move_file(tmp_path, path))
                    return -1;
            }
     sprintf(tmp_path, "%s/g%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
            if(!is_file_ok(tmp_path))
            {
                sprintf(path, "%s/runlog/pg%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
                if(move_file(tmp_path, path))
                    return -1;
            }
      sprintf(tmp_path, "%s/ig%s.out", (runtime_info->parameter_info)->tmp_dir,file_name);
            if(!is_file_ok(tmp_path))
            {
                sprintf(path, "%s/runlog/pig%s", (runtime_info->parameter_info)->log_dir, file_name + 1);
                if(move_file(tmp_path, path))
                    return -1;
            }
       return 0;
}
