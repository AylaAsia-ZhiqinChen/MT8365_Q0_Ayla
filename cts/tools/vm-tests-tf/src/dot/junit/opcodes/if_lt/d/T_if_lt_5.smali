.source "T_if_icmpge_5.java"
.class  public Ldot/junit/opcodes/if_lt/d/T_if_lt_5;
.super  Ljava/lang/Object;


.method public constructor <init>()V
.registers 1

       invoke-direct {v0}, Ljava/lang/Object;-><init>()V
       return-void
.end method

.method public run(ID)I
.registers 8

       if-lt v5, v6, :Label11
       const/16 v5, 1234
       return v5

:Label11
       const/4 v5, 1
       return v5
.end method
