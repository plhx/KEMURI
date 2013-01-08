#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#define ELF32_ORG 0x8048000

void add_esp(uint32_t n,FILE *f){
	if(n < 128){
		fwrite("\x83\xc4",1,2,f);
		fwrite(&n,1,1,f);
		return;
	}
	fwrite("\x81\xc4",1,2,f);
	fwrite(&n,1,4,f);
}

int main(void){
	char *buf;
	int c;
	uint32_t size,mem = 0;
	FILE *f;
	Elf32_Ehdr ehdr = {0};
	Elf32_Phdr phdr = {0};
	f = open_memstream(&buf,&size);
	while((c = fgetc(stdin)) != EOF){
		if(c == '^'){	/* xor */
			fwrite("\x58\x30\xc4\x50\x44",1,5,f);
			mem--;
		}
		else if(c == '~'){	/* not */
			fwrite("\xf6\x14\x24",1,3,f);
		}
		else if(c == '"'){	/* dup */
			fwrite("\x8b\x04\x24\x4c\x88\x04\x24",1,7,f);
			mem++;
		}
		else if(c == '\''){	/* rot */
			fwrite("\x0f\xb7\x44\x24\x01\x0f\xb6\x14\x24\x66\x89\x04\x24\x88\x54\x24\x02",1,17,f);
		}
		else if(c == '`'){	/* push */
			fwrite("\x68\x72\x6c\x64\x21\x68\x2c\x20\x77\x6f\x68\x65\x6c\x6c\x6f\x68\x00\x00\x00\x48\x83\xc4\x03",1,23,f);
			mem += 13;
		}
		else if(c == '|'){	/* print */
			fwrite("\xb8\x04\x00\x00\x00\xbb\x01\x00\x00\x00\x89\xe1\xba",1,13,f);
			fwrite(&mem,1,4,f);
			fwrite("\xcd\x80",1,2,f);
			add_esp(mem,f);
			mem = 0;
		}
	}
	if(mem)
		add_esp(mem,f);
	fwrite("\xb8\x01\x00\x00\x00\xcd\x80",1,7,f);
	fclose(f);
	*(uint32_t *)&ehdr.e_ident = *(uint32_t *)ELFMAG;
	ehdr.e_ident[EI_CLASS] = ELFCLASS32;
	ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
	ehdr.e_ident[EI_VERSION] = EV_CURRENT;
	ehdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
	ehdr.e_type = ET_EXEC;
	ehdr.e_machine = EM_386;
	ehdr.e_version = EV_CURRENT;
	ehdr.e_entry = ELF32_ORG + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	ehdr.e_phoff = sizeof(Elf32_Ehdr);
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_phentsize = sizeof(Elf32_Phdr);
	ehdr.e_phnum = 1;
	phdr.p_type = PT_LOAD;
	phdr.p_offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	phdr.p_vaddr = ELF32_ORG + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	phdr.p_filesz = size + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	phdr.p_memsz = size + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	phdr.p_flags = PF_X | PF_R;
	phdr.p_align = 0x0100;
	fwrite(&ehdr,sizeof(Elf32_Ehdr),1,stdout);
	fwrite(&phdr,sizeof(Elf32_Phdr),1,stdout);
	fwrite(buf,1,size,stdout);
	free(buf);
	return 0;
}
