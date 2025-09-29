
#define INT int
#define CONST const
#define DWORD unsigned int

extern "C" {

void    __fastcall D3DDevice_SetVertexShaderConstant1(INT Register, CONST void *pConstantData);
void    __fastcall D3DDevice_SetVertexShaderConstant1Fast(INT Register, CONST void *pConstantData);
void    __fastcall D3DDevice_SetVertexShaderConstant4(INT Register, CONST void *pConstantData);
void    __fastcall D3DDevice_SetVertexShaderConstantNotInline(INT Register, CONST void *pConstantData, DWORD ConstantCount);
void    __fastcall  D3DDevice_SetVertexShaderConstantNotInlineFast(INT Register, CONST void *pConstantData, DWORD ConstantCount);


void _stdcall D3DDevice_SetVertexShaderConstant(INT Register, CONST void *pConstantData, DWORD ConstantCount)
{
    if (ConstantCount == 1)
    {
        D3DDevice_SetVertexShaderConstant1(Register + 96, pConstantData);
    }
    else if (ConstantCount == 4)
    {
        D3DDevice_SetVertexShaderConstant4(Register + 96, pConstantData);
    }
    else
    {
        D3DDevice_SetVertexShaderConstantNotInline(Register + 96, pConstantData, 4 * ConstantCount);
    }
}

}