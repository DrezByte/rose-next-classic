USE [master]
GO
/****** Object:  Database [SHO_LOG]    Script Date: 9/12/2019 2:23:49 PM ******/
CREATE DATABASE [SHO_LOG];
GO

USE [SHO_LOG]
GO
/****** Object:  User [seven]    Script Date: 9/12/2019 2:23:49 PM ******/
CREATE USER [seven] FOR LOGIN [seven] WITH DEFAULT_SCHEMA=[dbo]
GO
ALTER ROLE [db_owner] ADD MEMBER [seven]
GO
ALTER ROLE [db_accessadmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_securityadmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_ddladmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_backupoperator] ADD MEMBER [seven]
GO
ALTER ROLE [db_datareader] ADD MEMBER [seven]
GO
ALTER ROLE [db_datawriter] ADD MEMBER [seven]
GO
/****** Object:  Table [dbo].[GS_ChangeAbility]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_ChangeAbility](
	[index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NULL,
	[Account] [nvarchar](20) NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[AbilityType] [smallint] NULL,
	[UsedPoint] [smallint] NULL,
	[BonusPoint] [smallint] NULL,
	[IP] [char](15) NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_ChangeAbility] PRIMARY KEY CLUSTERED 
(
	[index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_CharacterLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_CharacterLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[AccountName] [nvarchar](20) NULL,
	[CharName] [nvarchar](30) NULL,
	[DelAdd] [tinyint] NULL,
 CONSTRAINT [PK_GS_CharacterLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_CreateLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_CreateLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[ItemID] [varchar](10) NULL,
	[ItemName] [varchar](50) NULL,
	[Stuff1] [varchar](30) NULL,
	[Stuff2] [varchar](30) NULL,
	[Stuff3] [varchar](30) NULL,
	[Stuff4] [varchar](30) NULL,
	[Making] [tinyint] NULL,
	[Success] [tinyint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_CreateLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_DieLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_DieLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharName] [varchar](32) NOT NULL,
	[Money] [bigint] NULL,
	[CharLevel] [smallint] NULL,
	[Exp] [int] NULL,
	[KillPos] [varchar](24) NULL,
	[PosX] [int] NULL,
	[PosY] [int] NULL,
	[ObjectName] [varchar](32) NULL,
 CONSTRAINT [PK_GS_DieLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_GemmingLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_GemmingLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[ItemID] [varchar](10) NULL,
	[ItemName] [varchar](50) NULL,
	[JewelID] [varchar](10) NULL,
	[JewelName] [varchar](24) NULL,
	[Gemming] [tinyint] NULL,
	[Success] [tinyint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_GemmingLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_ItemLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_ItemLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[Action] [int] NOT NULL,
	[SbjAccount] [nvarchar](20) NOT NULL,
	[SbjCharID] [int] NOT NULL,
	[SbjCharName] [nvarchar](30) NOT NULL,
	[ItemID] [varchar](10) NULL,
	[ItemName] [varchar](50) NULL,
	[ItemCount] [smallint] NULL,
	[ItemSN] [bigint] NULL,
	[ItemOpt] [smallint] NULL,
	[Money] [bigint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
	[ObjAccount] [nvarchar](20) NULL,
	[ObjCharID] [int] NULL,
	[ObjCharName] [nvarchar](30) NULL,
	[SbjIP] [varchar](15) NOT NULL,
	[ObjIP] [varchar](15) NULL,
 CONSTRAINT [PK_GS_ItemLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_LevelUpLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_LevelUpLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[toLevel] [smallint] NULL,
	[BPoint] [smallint] NULL,
	[SPoint] [smallint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_LevelUpLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_PeriodicCHARLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_PeriodicCHARLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharName] [varchar](32) NOT NULL,
	[Channel] [tinyint] NULL,
	[CharLevel] [smallint] NULL,
	[Money] [bigint] NULL,
	[Exp] [int] NULL,
	[BPoint] [smallint] NULL,
	[SPoint] [smallint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_PeriodicCHARLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_QuestLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_QuestLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[QuestID] [int] NULL,
	[QuestDo] [tinyint] NULL,
 CONSTRAINT [PK_GS_QuestLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_SkillLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_SkillLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[SkillID] [int] NULL,
	[SkillName] [varchar](24) NULL,
	[SkillLevel] [smallint] NULL,
	[SPoint] [smallint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_SkillLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_UnionLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_UnionLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[iCharID] [int] NOT NULL,
	[szCharName] [nvarchar](30) NOT NULL,
	[nAction] [smallint] NOT NULL,
	[nCurUnion] [smallint] NOT NULL,
	[iCurPoint] [int] NOT NULL,
	[nAfterUnion] [smallint] NOT NULL,
	[iAfterPoint] [int] NOT NULL,
	[szLocation] [varchar](50) NULL,
	[iLocX] [int] NULL,
	[iLocY] [int] NULL,
 CONSTRAINT [PK_GS_UnionLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[GS_UpgradeLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GS_UpgradeLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharID] [int] NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[ItemID] [varchar](10) NULL,
	[ItemName] [varchar](50) NULL,
	[UpLevel] [smallint] NULL,
	[Success] [tinyint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_GS_UpgradeLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 95) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LS_LoginLog]    Script Date: 9/12/2019 2:23:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LS_LoginLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[Login] [tinyint] NULL,
	[AccountName] [nvarchar](30) NULL,
	[LoginIP] [varchar](15) NULL,
 CONSTRAINT [PK_LS_LoginLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblGS_ERROR]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblGS_ERROR](
	[Index] [int] NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[txtIP] [varchar](15) NULL,
	[txtACCOUNT] [nvarchar](20) NULL,
	[txtCHAR] [nvarchar](32) NULL,
	[txtFILE] [char](255) NULL,
	[intLINE] [int] NULL,
	[txtDESC] [nvarchar](512) NULL,
 CONSTRAINT [PK_tblGS_ERROR] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblGS_LOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblGS_LOG](
	[dateREG] [datetime] NOT NULL,
	[txtSUBJECT] [nvarchar](50) NOT NULL,
	[txtACTION] [varchar](30) NOT NULL,
	[txtITEM] [nvarchar](200) NULL,
	[intITEMSN] [bigint] NULL,
	[intMONEY] [bigint] NULL,
	[txtLOC] [nvarchar](100) NULL,
	[txtOBJECT] [nvarchar](50) NULL,
	[txtSBJIP] [varchar](40) NOT NULL,
	[txtOBJIP] [varchar](40) NULL,
	[txtDESC] [nvarchar](200) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WS_CheatLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WS_CheatLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[Account] [nvarchar](20) NOT NULL,
	[CharName] [nvarchar](30) NOT NULL,
	[ChannelNo] [tinyint] NULL,
	[CheatCode] [nvarchar](55) NULL,
 CONSTRAINT [PK_WS_CheatLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WS_ClanLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WS_ClanLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[CharName] [nvarchar](32) NULL,
	[ClanName] [nvarchar](20) NULL,
	[ClanLevel] [smallint] NULL,
	[Point] [int] NULL,
	[Success] [tinyint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
 CONSTRAINT [PK_WS_ClanLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[WS_LoginLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WS_LoginLog](
	[Index] [int] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NOT NULL,
	[Login] [tinyint] NULL,
	[CharName] [nvarchar](30) NULL,
	[Channel] [tinyint] NULL,
	[CharLevel] [smallint] NULL,
	[Money] [bigint] NULL,
	[Location] [varchar](24) NULL,
	[LocX] [int] NULL,
	[LocY] [int] NULL,
	[LoginIP] [varchar](15) NULL,
 CONSTRAINT [PK_WS_LoginLog] PRIMARY KEY CLUSTERED 
(
	[Index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[GS_ChangeAbility] ADD  CONSTRAINT [DF_GS_ChangeAbility_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_CharacterLog] ADD  CONSTRAINT [DF_GS_CharacterLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_CreateLog] ADD  CONSTRAINT [DF_GS_CreateLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_DieLog] ADD  CONSTRAINT [DF_GS_DieLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_GemmingLog] ADD  CONSTRAINT [DF_GS_GemmingLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_ItemLog] ADD  CONSTRAINT [DF_GS_ItemLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_ItemLog] ADD  CONSTRAINT [DF_GS_ItemLog_ItemOpt]  DEFAULT ((0)) FOR [ItemOpt]
GO
ALTER TABLE [dbo].[GS_LevelUpLog] ADD  CONSTRAINT [DF_GS_LevelUpLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_PeriodicCHARLog] ADD  CONSTRAINT [DF_GS_PeriodicCHARLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_QuestLog] ADD  CONSTRAINT [DF_GS_QuestLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_SkillLog] ADD  CONSTRAINT [DF_GS_SkillLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_UnionLog] ADD  CONSTRAINT [DF_GS_UnionLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[GS_UnionLog] ADD  CONSTRAINT [DF_GS_UnionLog_nCurUnion]  DEFAULT ((0)) FOR [nCurUnion]
GO
ALTER TABLE [dbo].[GS_UnionLog] ADD  CONSTRAINT [DF_GS_UnionLog_iCurPoint]  DEFAULT ((0)) FOR [iCurPoint]
GO
ALTER TABLE [dbo].[GS_UnionLog] ADD  CONSTRAINT [DF_GS_UnionLog_Union]  DEFAULT ((0)) FOR [nAfterUnion]
GO
ALTER TABLE [dbo].[GS_UnionLog] ADD  CONSTRAINT [DF_GS_UnionLog_UsePoint]  DEFAULT ((0)) FOR [iAfterPoint]
GO
ALTER TABLE [dbo].[GS_UpgradeLog] ADD  CONSTRAINT [DF_GS_UpgradeLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[tblGS_LOG] ADD  CONSTRAINT [DF_tblGS_LOG_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[tblGS_LOG] ADD  CONSTRAINT [DF_tblGS_LOG_intITEMSN]  DEFAULT ((0)) FOR [intITEMSN]
GO
ALTER TABLE [dbo].[tblGS_LOG] ADD  CONSTRAINT [DF_tblGS_LOG_intMONEY]  DEFAULT ((-1)) FOR [intMONEY]
GO
ALTER TABLE [dbo].[WS_CheatLog] ADD  CONSTRAINT [DF_WS_CheatLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[WS_ClanLog] ADD  CONSTRAINT [DF_WS_ClanLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
ALTER TABLE [dbo].[WS_LoginLog] ADD  CONSTRAINT [DF_WS_LoginLog_dateREG]  DEFAULT (getdate()) FOR [dateREG]
GO
/****** Object:  StoredProcedure [dbo].[AddChangeAbility]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE PROCEDURE [dbo].[AddChangeAbility] 
	@intCharID int , 
	@Account nvarchar( 20 ),
	@CharName nvarchar(30),
	@AbilityType smallint,
	@UsedPoint smallint,
	@IP char(15),
	@Location varchar(30),
	@LocX int , 
	@LocY int 
AS

Insert GS_ChangeAbility ( Account, CharID, CharName, AbilityType, UsedPoint, BonusPoint, IP, 

Location, LocX, LocY ) 
Values ( @Account, @intCharID, @CharName, @AbilityType, @UsedPoint, 0, @IP, @Location, @LocX, @LocY)




GO
/****** Object:  StoredProcedure [dbo].[AddCharacterLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddCharacterLog]
	@AccountName nvarchar(20),
	@CharName nvarchar(30),
	@DelAdd tinyint
AS

Insert Into GS_CharacterLog(
	dateREG,
	AccountName, CharName, DelAdd)
Values (Default,
	@AccountName, @CharName, @DelAdd)





GO
/****** Object:  StoredProcedure [dbo].[AddCheatLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE PROCEDURE [dbo].[AddCheatLog] 
	@Account nvarchar(20),
	@CharName nvarchar(30),
	@ChannelNo tinyint,
	@CheatCode nvarchar(55)
AS

Insert Into WS_CheatLog (Account, CharName, ChannelNo, CheatCode)
Values(@Account, @CharName, @ChannelNo, @CheatCode)




GO
/****** Object:  StoredProcedure [dbo].[AddClanLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE Proc [dbo].[AddClanLog]
	@CharName nvarchar(32),
	@ClanName nvarchar(20),
	@ClanLevel smallint,
	@Point int,
	@Success tinyint,
	@Location varchar(24),
	@LocX int,
	@LocY int
As

	Insert Into WS_ClanLog (dateREG,
		CharName,
		ClanName, ClanLevel, Point, Success,
		Location, LocX, LocY)
	Values (Default,
		@CharName,
		@ClanName, @ClanLevel, @Point, @Success,
		@Location, @LocX, @LocY)



GO
/****** Object:  StoredProcedure [dbo].[AddCreateLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddCreateLog]
	@CharID int,
	@CharName nvarchar(30),
	@ItemID varchar(10),
	@ItemName varchar(50),
	@Stuff1 varchar(24),
	@Stuff2 varchar(24),
	@Stuff3 varchar(24),
	@Stuff4 varchar(24),
	@Making tinyint,
	@Success tinyint,
	@Location varchar(24),
	@LocX int,
	@LocY int
AS

Insert Into GS_CreateLog (
	dateREG,
	CharID, CharName,
	ItemID, ItemName,
	Stuff1, Stuff2, Stuff3, Stuff4,
	Making, Success,
	Location, LocX, LocY)
Values (	Default,
	@CharID, @CharName,
	@ItemID, @ItemName,
	@Stuff1, @Stuff2, @Stuff3, @Stuff4,
	@Making, @Success,
	@Location, @LocX, @LocY )





GO
/****** Object:  StoredProcedure [dbo].[AddDieLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE Proc [dbo].[AddDieLog]
	@CharName varchar(32),
	@Money bigint,
	@KillPos varchar(24),
	@CharLevel smallint,
	@Exp int,
	@PosX int,
	@PosY int,
	@ObjectName varchar(32)
As

	Insert Into GS_DieLog (dateREG,
		CharName,
		[Money], CharLevel, [Exp],
		KillPos, PosX, PosY,
		ObjectName)
	Values(Default,
		@CharName,
		@Money, @CharLevel, @Exp,
		@KillPos, @PosX, @PosY,
		@ObjectName)



GO
/****** Object:  StoredProcedure [dbo].[AddGemmingLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE Proc [dbo].[AddGemmingLog]
	@CharID int,
	@CharName nvarchar(30),
	@ItemID varchar(10),
	@ItemName varchar(50),
	@JewelID varchar(10),
	@JewelName varchar(24),
	@Gemming tinyint,
	@Success tinyint,
	@Location varchar(24),
	@LocX int,
	@LocY int
AS

Insert Into GS_GemmingLog (
	dateREG,
	CharID, CharName,
	ItemID, ItemName, JewelID, JewelName,
	Gemming, Success,
	Location, LocX, LocY)
Values (	Default,
	@CharID, @CharName,
	@ItemID, @ItemName, @JewelID, @JewelName,
	@Gemming, @Success,
	@Location, @LocX, @LocY)




GO
/****** Object:  StoredProcedure [dbo].[AddItemLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




--[GS_ItemLog] Table
-- ItemOpt smallint NULL, Default(0) ??
-- AddItemLog SP ??

CREATE Proc [dbo].[AddItemLog]
	@Action int,
	@SbjAccount nvarchar(20),
	@SbjCharID int,
	@SbjCharName nvarchar(30),
	@ItemID varchar(10),
	@ItemName varchar(50),
	@ItemCount smallint,
	@ItemSN bigint,
	@ItemOpt smallint,
	@Money bigint,
	@Location varchar(24),
	@LocX int,
	@LocY int,
	@ObjAccount nvarchar(20),
	@ObjCharID int,
	@ObjCharName nvarchar(30),
	@SbjIP varchar(15),
	@ObjIP varchar(15)
AS

Insert Into GS_ItemLog (
	dateREG,
	[Action],
	SbjAccount, SbjCharID, SbjCharName,
	ItemID, ItemName, ItemCount, ItemSN,
	ItemOpt, [Money],
	Location, LocX, LocY,
	ObjAccount, ObjCharID, ObjCharName,
	SbjIP, ObjIP)
Values (Default,
	@Action,
	@SbjAccount, @SbjCharID, @SbjCharName,
	@ItemID, @ItemName, @ItemCount, @ItemSN,
	@ItemOpt, @Money,
	@Location, @LocX, @LocY,
	@ObjAccount, @ObjCharID, @ObjCharName,
	@SbjIP, @ObjIP)





GO
/****** Object:  StoredProcedure [dbo].[AddLevelUpLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddLevelUpLog]
	@CharID int,
	@CharName nvarchar(30),
	@toLevel smallint,
	@BPoint smallint,
	@SPoint smallint,
	@Location varchar(24),
	@LocX int,
	@LocY int
AS

Insert Into GS_LevelUpLog (
	dateREG,
	CharID, CharName,
	toLevel, BPoint, SPoint,
	Location, LocX, LocY)
Values (Default,
	@CharID, @CharName,
	@toLevel, @BPoint, @SPoint,
	@Location, @LocX, @LocY)





GO
/****** Object:  StoredProcedure [dbo].[AddLoginLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE Proc [dbo].[AddLoginLog]
	@Login tinyint,
	@CharName nvarchar(30),
	@Channel tinyint,
	@CharLevel smallint,
	@Money bigint,
	@Location varchar(24),
	@LocX int,
	@LocY int,
	@LoginIP varchar(15)
AS

Insert Into WS_LoginLog(
	dateREG,
	Login,
	CharName,
	Channel,
	CharLevel, [Money],
	Location, LocX, LocY,
	LoginIP)
Values (Default,
	@Login,
	@CharName, 
	@Channel,
	@CharLevel, @Money,
	@Location, @LocX, @LocY,
	@LoginIP)




GO
/****** Object:  StoredProcedure [dbo].[AddLoginLog_LS]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE Proc [dbo].[AddLoginLog_LS]
	@Login tinyint,
	@AccountName nvarchar(30),
	@LoginIP varchar(15)
AS

Insert Into LS_LoginLog(
	dateREG,
	Login,
	AccountName,
	LoginIP)
Values (getdate(),
	@Login,
	@AccountName, 
	@LoginIP)
GO
/****** Object:  StoredProcedure [dbo].[AddPeriodicCHARLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







Create Proc [dbo].[AddPeriodicCHARLog]
	@CharName varchar(32),
	@Channel tinyint,
	@CharLevel smallint,
	@Money bigint,
	@Exp int,
	@BPoint smallint,
	@SPoint smallint,
	@Location varchar(24),
	@LocX int,
	@LocY int
As

	Insert Into GS_PeriodicCHARLog(dateREG,
		CharName,
		Channel,
		CharLevel, [Money], [Exp], BPoint, SPoint,
		Location, LocX, LocY)
	Values (Default,
		@CharName,
		@Channel,
		@CharLevel, @Money, @Exp, @BPoint, @SPoint,
		@Location, @LocX, @LocY)






GO
/****** Object:  StoredProcedure [dbo].[AddQuestLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddQuestLog]
	@CharID int,
	@CharName nvarchar(30),
	@QuestID int,
	@QuestDo tinyint
AS

Insert Into GS_QuestLog(
	dateREG,
	CharID, CharName,
	QuestID, QuestDo)
Values (Default,
	@CharID, @CharName,
	@QuestID, @QuestDo)





GO
/****** Object:  StoredProcedure [dbo].[AddSkillLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddSkillLog]
	@CharID int,
	@CharName nvarchar(30),
	@SkillID int,
	@SkillName varchar(24),
	@SkillLevel smallint,
	@SPoint smallint,
	@Location varchar(24),
	@LocX int,
	@LocY int
AS

Insert Into GS_SkillLog(
	dateREG,
	CharID, CharName,
	SkillID, SkillName, SkillLevel, SPoint,
	Location, LocX, LocY)
Values (Default,
	@CharID, @CharName,
	@SkillID, @SkillName, @SkillLevel, @SPoint,
	@Location, @LocX, @LocY)





GO
/****** Object:  StoredProcedure [dbo].[AddUnionLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE Proc [dbo].[AddUnionLog]
	@iCharID int,
	@szCharName nvarchar(30),
	@nAction smallint,
	@nCurUnion smallint,
	@iCurPoint int,
	@nAfterUnion smallint,
	@iAfterPoint int,
	@szLocation varchar(50),
	@iLocX int,
	@iLocY int
AS

Insert Into GS_UnionLog (
	dateREG,
	iCharID, szCharName,
	nAction, 
	nCurUnion, iCurPoint, 
	nAfterUnion, iAfterPoint,
	szLocation, iLocX, iLocY)
Values (Default,
	@iCharID, @szCharName,
	@nAction, 
	@nCurUnion, @iCurPoint,
	@nAfterUnion, @iAfterPoint,
	@szLocation, @iLocX, @iLocY)




GO
/****** Object:  StoredProcedure [dbo].[AddUpgradeLog]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO





Create Proc [dbo].[AddUpgradeLog]
	@CharID int,
	@CharName nvarchar(30),
	@ItemID varchar(10),
	@ItemName varchar(50),
	@UpLevel smallint,
	@Success tinyint,
	@Location varchar(24),
	@LocX int,
	@LocY int
AS

Insert Into GS_UpgradeLog(
	dateREG,
	CharID, CharName,
	ItemID, ItemName,
	UpLevel, Success,
	Location, LocX, LocY)
Values  ( Default,
	@CharID, @CharName,
	@ItemID, @ItemName,
	@UpLevel, @Success,
	@Location, @LocX, @LocY)





GO
/****** Object:  StoredProcedure [dbo].[gs_DefLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[gs_DefLOG]
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM )






GO
/****** Object:  StoredProcedure [dbo].[gs_DescLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO









CREATE PROCEDURE [dbo].[gs_DescLOG]
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szDESC	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM,txtDESC ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szDESC )






GO
/****** Object:  StoredProcedure [dbo].[gs_LogInOut]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO









CREATE PROCEDURE [dbo].[gs_LogInOut]
	@szSUB	nvarchar(50),
	@szOBJ	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ )






GO
/****** Object:  StoredProcedure [dbo].[gs_M_DefLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO








CREATE PROCEDURE [dbo].[gs_M_DefLOG]
	@iMoney	int,
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM )





GO
/****** Object:  StoredProcedure [dbo].[gs_M_DescLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO








CREATE PROCEDURE [dbo].[gs_M_DescLOG]
	@iMoney	int,
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szDESC	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM,txtDESC ) VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szDESC )





GO
/****** Object:  StoredProcedure [dbo].[gs_M_LogInOut]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO








CREATE PROCEDURE [dbo].[gs_M_LogInOut]
	@iMoney	int,
	@szSUB	nvarchar(50),
	@szOBJ	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT ) VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ )





GO
/****** Object:  StoredProcedure [dbo].[gs_M_ObjDescLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO








CREATE PROCEDURE [dbo].[gs_M_ObjDescLOG]
	@iMoney	int,
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szOBJ	nvarchar(50),
	@szOBJIP	nvarchar(40),
	@szDESC	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP, txtDESC ) VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP, @szDESC )





GO
/****** Object:  StoredProcedure [dbo].[gs_M_ObjLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO








CREATE PROCEDURE [dbo].[gs_M_ObjLOG]
	@iMoney	int,
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szOBJ	nvarchar(50),
	@szOBJIP	nvarchar(40)
AS
	INSERT tblGS_LOG ( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP )





GO
/****** Object:  StoredProcedure [dbo].[gs_ObjDescLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO









CREATE PROCEDURE [dbo].[gs_ObjDescLOG]
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szOBJ	nvarchar(50),
	@szOBJIP	nvarchar(40),
	@szDESC	nvarchar(200)
AS
	INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP, txtDESC ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP, @szDESC )






GO
/****** Object:  StoredProcedure [dbo].[gs_ObjLOG]    Script Date: 9/12/2019 2:23:50 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO









CREATE PROCEDURE [dbo].[gs_ObjLOG]
	@szSUB	nvarchar(50),
	@szSBJIP	varchar(40),
	@szACT	varchar(30),
	@szLOC	nvarchar(100),
	@szITEM	nvarchar(200),
	@szOBJ	nvarchar(50),
	@szOBJIP	nvarchar(40)
AS
	INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP )






GO
USE [master]
GO
ALTER DATABASE [SHO_LOG] SET  READ_WRITE 
GO
