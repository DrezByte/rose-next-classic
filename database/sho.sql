USE [master]
GO
/****** Object:  Database [SHO]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE DATABASE [SHO]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'SHO', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL14.SQLEXPRESS\MSSQL\DATA\SHO.mdf' , SIZE = 8192KB , MAXSIZE = UNLIMITED, FILEGROWTH = 65536KB )
 LOG ON 
( NAME = N'SHO_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL14.SQLEXPRESS\MSSQL\DATA\SHO_log.ldf' , SIZE = 204800KB , MAXSIZE = 2048GB , FILEGROWTH = 65536KB )
GO
ALTER DATABASE [SHO] SET COMPATIBILITY_LEVEL = 140
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
	EXEC [SHO].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [SHO] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [SHO] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [SHO] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [SHO] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [SHO] SET ARITHABORT OFF 
GO
ALTER DATABASE [SHO] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [SHO] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [SHO] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [SHO] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [SHO] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [SHO] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [SHO] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [SHO] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [SHO] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [SHO] SET  DISABLE_BROKER 
GO
ALTER DATABASE [SHO] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [SHO] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [SHO] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [SHO] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [SHO] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [SHO] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [SHO] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [SHO] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [SHO] SET  MULTI_USER 
GO
ALTER DATABASE [SHO] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [SHO] SET DB_CHAINING OFF 
GO
ALTER DATABASE [SHO] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [SHO] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO
ALTER DATABASE [SHO] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [SHO] SET QUERY_STORE = OFF
GO
USE [SHO]
GO
/****** Object:  User [seven]    Script Date: 9/12/2019 2:20:43 PM ******/
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
/****** Object:  Table [dbo].[tblGS_AVATAR]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblGS_AVATAR]
(
	[intCharID] [int] IDENTITY(1,1) NOT NULL,
	[txtACCOUNT] [nvarchar](20) NOT NULL,
	[txtNAME] [nvarchar](30) NOT NULL,
	[btLEVEL] [smallint] NULL,
	[intMoney] [bigint] NULL,
	[dwRIGHT] [int] NULL,
	[binBasicE] [binary](96) NOT NULL,
	[binBasicI] [binary](32) NOT NULL,
	[binBasicA] [binary](48) NOT NULL,
	[binGrowA] [binary](384) NOT NULL,
	[binSkillA] [binary](384) NOT NULL,
	[blobQUEST] [binary](1024) NULL,
	[blobINV] [binary](2048) NOT NULL,
	[binHotICON] [binary](128) NULL,
	[dwDelTIME] [int] NULL,
	[binWishLIST] [binary](256) NULL,
	[dwOPTION] [int] NULL,
	[intJOB] [smallint] NOT NULL,
	[dwRegTIME] [int] NOT NULL,
	[dwPartyIDX] [int] NULL,
	[dwItemSN] [int] NULL,
	[intDataVER] [smallint] NOT NULL,
	[txtCharName] [nvarchar](30) NULL,
	[binSkillB] [binary](240) NULL,
	CONSTRAINT [PK_tblGS_AVATAR] PRIMARY KEY CLUSTERED 
(
	[txtNAME] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblGS_BANK]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblGS_BANK]
(
	[txtACCOUNT] [nvarchar](20) NOT NULL,
	[blobITEMS] [binary](2250) NULL,
	[intREWARD] [money] NULL,
	[txtPASSWORD] [nvarchar](10) NULL,
	CONSTRAINT [PK_tblGS_BANK] PRIMARY KEY CLUSTERED 
(
	[txtACCOUNT] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblWS_CLAN]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblWS_CLAN]
(
	[intID] [int] IDENTITY(1,1) NOT NULL,
	[txtNAME] [nchar](20) NOT NULL,
	[txtDESC] [nchar](255) NULL,
	[intMarkIDX1] [smallint] NOT NULL,
	[intMarkIDX2] [smallint] NULL,
	[intLEVEL] [smallint] NULL,
	[intPOINT] [int] NULL,
	[intAlliedID] [int] NULL,
	[intRATE] [smallint] NULL,
	[intMoney] [bigint] NULL,
	[binDATA] [binary](1024) NULL,
	[txtMSG] [nvarchar](368) NULL,
	[intMarkCRC] [smallint] NULL,
	[intMarkLEN] [smallint] NULL,
	[binMark] [binary](1024) NULL,
	[dateMarkREG] [datetime] NULL,
	CONSTRAINT [PK_tbl_WSCLAN] PRIMARY KEY CLUSTERED 
(
	[intID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblWS_ClanCHAR]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblWS_ClanCHAR]
(
	[txtCharNAME] [nchar](30) NOT NULL,
	[intClanID] [int] NOT NULL,
	[intPOINT] [int] NULL,
	[intPOS] [int] NULL,
	CONSTRAINT [PK_tblWS_ClanCHAR] PRIMARY KEY CLUSTERED 
(
	[txtCharNAME] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblWS_FRIEND]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblWS_FRIEND]
(
	[intCharID] [int] NOT NULL,
	[intFriendCNT] [smallint] NOT NULL,
	[blobFRIENDS] [binary](1024) NULL,
	CONSTRAINT [PK_tblWS_FRIEND] PRIMARY KEY CLUSTERED 
(
	[intCharID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblWS_MEMO]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblWS_MEMO]
(
	[intSN] [bigint] IDENTITY(1,1) NOT NULL,
	[dwDATE] [int] NOT NULL,
	[txtNAME] [nvarchar](30) NOT NULL,
	[txtFROM] [nvarchar](30) NOT NULL,
	[txtMEMO] [nvarchar](255) NOT NULL,
	CONSTRAINT [PK_tblWS_MEMO] PRIMARY KEY CLUSTERED 
(
	[intSN] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tblWS_VAR]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblWS_VAR]
(
	[txtNAME] [nvarchar](70) NOT NULL,
	[dateUPDATE] [datetime] NOT NULL,
	[binDATA] [varbinary](1024) NOT NULL,
	CONSTRAINT [PK_tblWS_VAR] PRIMARY KEY CLUSTERED 
(
	[txtNAME] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
) ON [PRIMARY]
GO

/****** Object:  Table [dbo].[WS_CheatLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[WS_CheatLog]
(
	[Index] [int] NULL,
	[dateREG] [datetime] NULL,
	[Account] [nvarchar](20) NULL,
	[CharName] [nvarchar](30) NULL,
	[ChannelNo] [char](1) NULL,
	[CheatCode] [nvarchar](55) NULL
) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tblGS_AVATAR]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblGS_AVATAR] ON [dbo].[tblGS_AVATAR]
(
	[txtACCOUNT] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
/****** Object:  Index [IX_tblGS_AVATAR_1]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE UNIQUE NONCLUSTERED INDEX [IX_tblGS_AVATAR_1] ON [dbo].[tblGS_AVATAR]
(
	[intCharID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
/****** Object:  Index [IX_tblGS_AVATAR_2]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblGS_AVATAR_2] ON [dbo].[tblGS_AVATAR]
(
	[btLEVEL] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
/****** Object:  Index [IX_tblGS_AVATAR_3]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblGS_AVATAR_3] ON [dbo].[tblGS_AVATAR]
(
	[intMoney] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
/****** Object:  Index [IX_tblGS_AVATAR_4]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblGS_AVATAR_4] ON [dbo].[tblGS_AVATAR]
(
	[intJOB] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tblWS_CLAN]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblWS_CLAN] ON [dbo].[tblWS_CLAN]
(
	[txtNAME] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
/****** Object:  Index [IX_tblWS_ClanCHAR]    Script Date: 9/12/2019 2:20:43 PM ******/
CREATE NONCLUSTERED INDEX [IX_tblWS_ClanCHAR] ON [dbo].[tblWS_ClanCHAR]
(
	[intClanID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90) ON [PRIMARY]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_btLEVEL]  DEFAULT ((0)) FOR [btLEVEL]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_intMoney]  DEFAULT ((0)) FOR [intMoney]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwRIGHT]  DEFAULT ((0)) FOR [dwRIGHT]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_binSkillA]  DEFAULT (0x0B000C0010000000140029002A002B001E00) FOR [binSkillA]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwDelTIME]  DEFAULT ((0)) FOR [dwDelTIME]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwOPTION]  DEFAULT ((0)) FOR [dwOPTION]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_intJOB]  DEFAULT ((0)) FOR [intJOB]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwRegTIME]  DEFAULT ((0)) FOR [dwRegTIME]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwPartyIDX]  DEFAULT ((0)) FOR [dwPartyIDX]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR_dwItemSN]  DEFAULT ((0)) FOR [dwItemSN]
GO
ALTER TABLE [dbo].[tblGS_AVATAR] ADD  CONSTRAINT [DF_tblGS_AVATAR]  DEFAULT ((0)) FOR [intDataVER]
GO
ALTER TABLE [dbo].[tblGS_BANK] ADD  CONSTRAINT [DF_tblGS_BANK_intREWARD]  DEFAULT ((0)) FOR [intREWARD]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intMarkIDX]  DEFAULT ((0)) FOR [intMarkIDX1]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intMarkIDX2]  DEFAULT ((0)) FOR [intMarkIDX2]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intLEVEL]  DEFAULT ((1)) FOR [intLEVEL]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intPOINT]  DEFAULT ((0)) FOR [intPOINT]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intAlliedID1]  DEFAULT ((0)) FOR [intAlliedID]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intRATE]  DEFAULT ((100)) FOR [intRATE]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_GUILD_intMoney]  DEFAULT ((0)) FOR [intMoney]
GO
ALTER TABLE [dbo].[tblWS_CLAN] ADD  CONSTRAINT [DF_tblWS_CLAN_intMarkLEN]  DEFAULT ((0)) FOR [intMarkLEN]
GO
ALTER TABLE [dbo].[tblWS_ClanCHAR] ADD  CONSTRAINT [DF_tblWS_GuildCHAR_intPOINT]  DEFAULT ((0)) FOR [intPOINT]
GO
ALTER TABLE [dbo].[tblWS_ClanCHAR] ADD  CONSTRAINT [DF_tblWS_GuildCHAR_intPOS]  DEFAULT ((0)) FOR [intPOS]
GO
ALTER TABLE [dbo].[tblWS_FRIEND] ADD  CONSTRAINT [DF_tblWS_FRIEND_intFriendCNT]  DEFAULT ((0)) FOR [intFriendCNT]
GO
/****** Object:  StoredProcedure [dbo].[AddCharacterLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


Create Proc [dbo].[AddCharacterLog]
	@AccountName nvarchar(20),
	@CharName nvarchar(30),
	@DelAdd tinyint
AS
Insert Into GS_CharacterLog
	( dateREG, AccountName, CharName, DelAdd)
Values
	(Default, @AccountName, @CharName, @DelAdd) 

GO
/****** Object:  StoredProcedure [dbo].[AddCheatLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into WS_CheatLog
	(Account, CharName, ChannelNo, CheatCode)
Values(@Account, @CharName, @ChannelNo, @CheatCode) 

GO
/****** Object:  StoredProcedure [dbo].[AddClanLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
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
Insert Into WS_ClanLog
	(dateREG, CharName, ClanName, ClanLevel, Point, Success, Location, LocX, LocY)
Values
	(Default, @CharName, @ClanName, @ClanLevel, @Point, @Success, @Location, @LocX, @LocY) 

GO
/****** Object:  StoredProcedure [dbo].[AddCreateLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_CreateLog
	( dateREG, CharID, CharName, ItemID, ItemName, Stuff1, Stuff2, Stuff3, Stuff4, Making, Success, Location, LocX, LocY)
Values
	( Default, @CharID, @CharName, @ItemID, @ItemName, @Stuff1, @Stuff2, @Stuff3, @Stuff4, @Making, @Success, @Location, @LocX, @LocY ) 

GO
/****** Object:  StoredProcedure [dbo].[AddDieLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
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
	@ObjectName varchar(50)
As
Insert Into GS_DieLog
	(dateREG, CharName, [Money], CharLevel, [Exp], KillPos, PosX, PosY, ObjectName)
Values(Default, @CharName, @Money, @CharLevel, @Exp, @KillPos, @PosX, @PosY, @ObjectName) 

GO
/****** Object:  StoredProcedure [dbo].[AddGemmingLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_GemmingLog
	( dateREG, CharID, CharName, ItemID, ItemName, JewelID, JewelName, Gemming, Success, Location, LocX, LocY)
Values
	( Default, @CharID, @CharName, @ItemID, @ItemName, @JewelID, @JewelName, @Gemming, @Success, @Location, @LocX, @LocY) 

GO
/****** Object:  StoredProcedure [dbo].[AddItemLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE Proc [dbo].[AddItemLog]
	@Action int,
	@SbjAccount nvarchar(20),
	@SbjCharID int,
	@SbjCharName nvarchar(30),
	@ItemID varchar(10),
	@ItemName varchar(50),
	@ItemCount smallint,
	@ItemSN bigint,
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
Insert Into GS_ItemLog
	( dateREG, [Action], SbjAccount, SbjCharID, SbjCharName, ItemID, ItemName, ItemCount, ItemSN, [Money], Location, LocX, LocY, ObjAccount, ObjCharID, ObjCharName, SbjIP, ObjIP)
Values
	(Default, @Action, @SbjAccount, @SbjCharID, @SbjCharName, @ItemID, @ItemName, @ItemCount, @ItemSN, @Money, @Location, @LocX, @LocY, @ObjAccount, @ObjCharID, @ObjCharName, @SbjIP, @ObjIP) 

GO
/****** Object:  StoredProcedure [dbo].[AddLevelUpLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_LevelUpLog
	( dateREG, CharID, CharName, toLevel, BPoint, SPoint, Location, LocX, LocY)
Values
	(Default, @CharID, @CharName, @toLevel, @BPoint, @SPoint, @Location, @LocX, @LocY) 

GO
/****** Object:  StoredProcedure [dbo].[AddLoginLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into WS_LoginLog
	( dateREG, Login, CharName, Channel, CharLevel, [Money], Location, LocX, LocY, LoginIP)
Values
	(Default, @Login, @CharName, @Channel, @CharLevel, @Money, @Location, @LocX, @LocY, @LoginIP) 

GO
/****** Object:  StoredProcedure [dbo].[AddPeriodicCHARLog]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
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
Insert Into GS_PeriodicCHARLog
	(dateREG, CharName, Channel, CharLevel, [Money], [Exp], BPoint, SPoint, Location, LocX, LocY)
Values
	(Default, @CharName, @Channel, @CharLevel, @Money, @Exp, @BPoint, @SPoint, @Location, @LocX, @LocY) 

GO
/****** Object:  StoredProcedure [dbo].[AddQuestLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_QuestLog
	( dateREG, CharID, CharName, QuestID, QuestDo)
Values
	(Default, @CharID, @CharName, @QuestID, @QuestDo) 

GO
/****** Object:  StoredProcedure [dbo].[AddSkillLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_SkillLog
	( dateREG, CharID, CharName, SkillID, SkillName, SkillLevel, SPoint, Location, LocX, LocY)
Values
	(Default, @CharID, @CharName, @SkillID, @SkillName, @SkillLevel, @SPoint, @Location, @LocX, @LocY) 

GO
/****** Object:  StoredProcedure [dbo].[AddUpgradeLog]    Script Date: 9/12/2019 2:20:43 PM ******/
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
Insert Into GS_UpgradeLog
	( dateREG, CharID, CharName, ItemID, ItemName, UpLevel, Success, Location, LocX, LocY)
Values
	( Default, @CharID, @CharName, @ItemID, @ItemName, @UpLevel, @Success, @Location, @LocX, @LocY) 

GO

/****** Object:  StoredProcedure [dbo].[gs_M_DefLOG]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[gs_M_DefLOG]
	@iMoney int,
	@szSUB nvarchar(32),
	@szSBJIP varchar(15),
	@szACT varchar(24),
	@szLOC nvarchar(50),
	@szITEM nvarchar(200)
AS
INSERT tblGS_LOG
	( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM )
VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM ) 

GO
/****** Object:  StoredProcedure [dbo].[gs_M_DescLOG]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[gs_M_DescLOG]
	@iMoney int,
	@szSUB nvarchar(32),
	@szSBJIP varchar(15),
	@szACT varchar(24),
	@szLOC nvarchar(50),
	@szITEM nvarchar(200),
	@szDESC nvarchar(200)
AS
INSERT tblGS_LOG
	( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM,txtDESC )
VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szDESC ) 

GO
/****** Object:  StoredProcedure [dbo].[gs_M_LogInOut]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[gs_M_LogInOut]
	@iMoney int,
	@szSUB nvarchar(32),
	@szOBJ nvarchar(32),
	@szSBJIP varchar(15),
	@szACT varchar(24),
	@szLOC nvarchar(50),
	@szITEM nvarchar(200)
AS
INSERT tblGS_LOG
	( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT )
VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szOBJ ) 

GO
/****** Object:  StoredProcedure [dbo].[gs_M_ObjDescLOG]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[gs_M_ObjDescLOG]
	@iMoney int,
	@szSUB nvarchar(32),
	@szSBJIP varchar(15),
	@szACT varchar(24),
	@szLOC nvarchar(50),
	@szITEM nvarchar(200),
	@szOBJ nvarchar(32),
	@szOBJIP nvarchar(15),
	@szDESC nvarchar(200)
AS
INSERT tblGS_LOG
	( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP, txtDESC )
VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szOBJ, @szOBJIP, @szDESC ) 

GO
/****** Object:  StoredProcedure [dbo].[gs_M_ObjLOG]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[gs_M_ObjLOG]
	@iMoney int,
	@szSUB nvarchar(32),
	@szSBJIP varchar(15),
	@szACT varchar(24),
	@szLOC nvarchar(50),
	@szITEM nvarchar(200),
	@szOBJ nvarchar(32),
	@szOBJIP nvarchar(15)
AS
INSERT tblGS_LOG
	( dateREG, intMoney, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP )
VALUES( default, @iMoney, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szOBJ, @szOBJIP )

GO
/****** Object:  StoredProcedure [dbo].[gs_SelectBANK]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.gs_SelectBANK    Script Date: 10/24/2005 4:10:57 PM ******/




CREATE PROCEDURE [dbo].[gs_SelectBANK]
	@szAccount  nvarchar(30)
AS
SELECT *
FROM tblGS_BANK
WHERE txtACCOUNT=@szAccount





GO
/****** Object:  StoredProcedure [dbo].[gs_SelectCHAR]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.gs_SelectCHAR    Script Date: 10/24/2005 4:10:57 PM ******/




CREATE PROCEDURE [dbo].[gs_SelectCHAR]
	@szCharName	nvarchar(30)
AS
SELECT *
FROM tblGS_AVATAR
WHERE txtNAME=@szCharName





GO

/****** Object:  StoredProcedure [dbo].[ws_CharDELETE]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_CharDELETE    Script Date: 10/24/2005 4:10:57 PM ******/




CREATE PROCEDURE [dbo].[ws_CharDELETE]
	@szCharName	nvarchar(30)
AS
DECLARE @del_error1 int;
DECLARE @del_error2 int;
DECLARE @del_error3 int;
DECLARE @del_error4 int;

DECLARE @iCharID	int;

-- ??? ??? ID??
SELECT @iCharID=intCharID
from tblGS_AVATAR
where txtNAME = @szCharName;
IF @@ROWCOUNT <= 0 
		RETURN -1;
-- ???? ??
BEGIN TRAN del_char

DELETE from tblWS_FRIEND WHERE intCharID = @iCharID;
SET @del_error1 = @@ERROR;

DELETE FROM tblGS_AVATAR WHERE txtNAME=@szCharName;
SET @del_error2 = @@ERROR;

DELETE FROM tblWS_MEMO   WHERE txtNAME=@szCharName;
SET @del_error3 = @@ERROR;

DELETE FROM tblWS_ClanCHAR WHERE txtCharNAME=@szCharName;
SET @del_error4 = @@ERROR;

IF @del_error1 = 0 AND @del_error2 = 0 AND @del_error3 = 0 AND @del_error4 = 0
	BEGIN
	COMMIT TRAN del_char;
	RETURN 0;
END

ROLLBACK TRAN del_char;
RETURN -2;





GO
/****** Object:  StoredProcedure [dbo].[ws_ClanBinUPDATE]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_ClanBinUPDATE    Script Date: 10/24/2005 4:10:57 PM ******/


CREATE PROCEDURE [dbo].[ws_ClanBinUPDATE]
	@iClanID	int,
	@binDAT	binary(1024)
AS
BEGIN TRAN upt_data

UPDATE tblWS_CLAN SET binDATA=@binDAT WHERE intID = @iClanID;

IF @@ERROR <> 0
	BEGIN
	ROLLBACK TRAN upt_data;
	RETURN 0;
END

COMMIT TRAN upt_data;
RETURN 1;

GO
/****** Object:  StoredProcedure [dbo].[ws_ClanCharADD]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanCharADD    Script Date: 10/24/2005 4:10:57 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanCharADD]
	@szCharName	nchar(30),
	@iClanID	int,
	@iClanPOS	int
AS
SELECT intClanID
FROM tblWS_ClanCHAR
where txtCharNAME=@szCharName;
IF @@ROWCOUNT >= 1
		RETURN -1;

BEGIN TRAN ins_char
INSERT tblWS_ClanCHAR
	(txtCharNAME, intClanID, intPOS )
VALUES( @szCharName, @iClanID, @iClanPOS );

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN ins_char;
	RETURN 0;
END

ROLLBACK TRAN ins_char;
RETURN -2;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanCharADJ]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanCharADJ    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanCharADJ]
	@szCharName	nchar(30),
	@iAdjPoint	int,
	@iAdjPos	int
AS
DECLARE	@iCurPoint	int;
DECLARE	@iCurPos	int;

SELECT @iCurPoint=intPOINT, @iCurPos=intPOS
FROM tblWS_ClanCHAR
where txtCharNAME=@szCharName;
IF @@ROWCOUNT <> 1
		RETURN -1;

BEGIN TRAN upd_char
SET @iCurPoint = @iCurPoint + @iAdjPoint;
SET @iCurPos   = @iCurPos	+ @iAdjPos;

UPDATE tblWS_ClanCHAR SET intPOINT=@iCurPoint, intPOS=@iCurPos where txtCharNAME=@szCharName;

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN upd_char;
	RETURN 0;
END

ROLLBACK TRAN upd_char;
RETURN -2;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanCharALL]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanCharALL    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanCharALL]
	@iClanID	int
AS
SELECT txtCharNAME, intPOINT, intPOS
from tblWS_ClanCHAR
where intClanID = @iClanID;
	-- RETURN @@ROWCOUNT;





GO
/****** Object:  StoredProcedure [dbo].[ws_ClanCharDEL]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanCharDEL    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanCharDEL]
	@szCharName	nchar(30)
AS
SELECT intClanID
FROM tblWS_ClanCHAR
where txtCharNAME=@szCharName;
IF @@ROWCOUNT < 1
		RETURN -1;
-- not found

BEGIN TRAN del_char
DELETE from tblWS_ClanCHAR where txtCharNAME = @szCharName;

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN del_char;
	RETURN 0;
END

ROLLBACK TRAN del_char;
RETURN -2;			-- db error






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanCharGET]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanCharGET    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanCharGET]
	@szCharName	nchar(30)
AS
SELECT intClanID, intPOINT, intPOS
from tblWS_ClanCHAR
where txtCharNAME = @szCharName;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanDELETE]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanDELETE    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanDELETE]
	@szClanName	nvarchar(20)
AS
DECLARE @del_error1 int;
DECLARE @del_error2 int;

DECLARE @iClanID	int;

-- ??? ??ID??
SELECT @iClanID=intID
from tblWS_CLAN
where txtNAME = @szClanName;
IF @@ROWCOUNT <= 0 
		RETURN -1;

BEGIN TRAN del_clan

DELETE from tblWS_ClanCHAR where intClanID = @iClanID
SET @del_error1 = @@ERROR;

DELETE from tblWS_CLAN where intID = @iClanID;--txtNAME = @szClanName;
SET @del_error2 = @@ERROR;

IF @del_error1 = 0 AND @del_error2 = 0
	BEGIN
	COMMIT TRAN del_clan;
	SELECT @iClanID;
	RETURN 0;
END

ROLLBACK TRAN del_clan
RETURN -2;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanINSERT]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanINSERT    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanINSERT]
	@szClanName	nvarchar(20),
	@szClanDesc	nvarchar(255),
	@iMark1	int,
	@iMark2	int
AS
SELECT intID
FROM tblWS_CLAN
WHERE txtNAME=@szClanName;
IF @@ROWCOUNT >= 1
		RETURN -1;

BEGIN TRAN ins_clan
INSERT tblWS_CLAN
	(txtNAME, txtDESC, intMarkIdx1, intMarkIdx2)
VALUES(
		@szClanName,
		@szClanDesc,
		@iMark1,
		@iMark2 );

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN ins_clan;
	SELECT intID
	FROM tblWS_CLAN
	WHERE txtNAME=@szClanName;
	RETURN 0;
END

ROLLBACK TRAN ins_clan
RETURN -2;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanMarkUPDATE]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/****** Object:  Stored Procedure dbo.ws_ClanMarkUPDATE    Script Date: 10/24/2005 4:10:58 PM ******/



CREATE   PROCEDURE [dbo].[ws_ClanMarkUPDATE]
	@iClanID	int,
	@iDataCRC	int,
	@iDataLEN	int,
	@binDATA	binary(1024)
AS
BEGIN TRAN upt_mark

DECLARE @dateCur	datetime;
DECLARE @intRowCount int
;

SET @dateCur = getdate();

UPDATE tblWS_CLAN SET intMarkCRC=@iDataCRC, intMarkLEN=@iDataLEN, binMARK=@binDATA, dateMarkREG=@dateCur  
	WHERE intID = @iClanID
;

set @intRowCount = @@ROWCOUNT

IF @@ERROR <> 0
	BEGIN
	ROLLBACK TRAN upt_mark;
	RETURN -1;
END

COMMIT TRAN upt_mark;
RETURN @intRowCount;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanMOTD]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanMOTD    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanMOTD]
	@iClanID	int,
	@szMessage	nvarchar(368)
AS

BEGIN TRAN upd_clan

UPDATE tblWS_CLAN SET txtMSG=@szMessage where intID = @iClanID;

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN upd_clan;
	RETURN 0;
END

ROLLBACK TRAN upd_clan
RETURN -1;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanSELECT]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanSELECT    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanSELECT]
	@iClanID	int
AS
SELECT *
from tblWS_CLAN
where intID = @iClanID;






GO
/****** Object:  StoredProcedure [dbo].[ws_ClanSLOGAN]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_ClanSLOGAN    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_ClanSLOGAN]
	@iClanID	int,
	@szMessage	nvarchar(255)
AS

BEGIN TRAN upd_clan

UPDATE tblWS_CLAN SET txtDESC=@szMessage where intID = @iClanID;

IF @@ERROR = 0
	BEGIN
	COMMIT TRAN upd_clan;
	RETURN 0;
END

ROLLBACK TRAN upd_clan
RETURN -1;





GO
/****** Object:  StoredProcedure [dbo].[ws_ClanUPDATE]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_ClanUPDATE    Script Date: 10/24/2005 4:10:57 PM ******/




--Create Proc

CREATE PROCEDURE [dbo].[ws_ClanUPDATE]
	@iClanID	int,
	@szField	varchar(20),
	@iAdjValue	int
AS
DECLARE	@szQry varchar(500)

SET @szQry = 'UPDATE tblWS_CLAN SET ' + @szField + ' = ' + @szField + ' + ' + cast(@iAdjValue as varchar) + ' WHERE intID= ' + cast ( @iClanID as varchar )

BEGIN TRAN upt_clan
EXEC (  @szQry )

IF @@ERROR <> 0
	BEGIN
	ROLLBACK TRAN upt_clan;
	RETURN 0;
END

COMMIT TRAN upt_clan;

SET @szQry = 'SELECT ' + @szField + ' FROM tblWS_CLAN WHERE intID= ' + cast ( @iClanID as varchar )
EXEC ( @szQry  )
RETURN 1;





GO
/****** Object:  StoredProcedure [dbo].[ws_DeleteCHAR]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_DeleteCHAR    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_DeleteCHAR]
	@szCharName	nvarchar(30),
	@iCharID	int
AS
DELETE FROM tblWS_FRIEND WHERE intCharID=@iCharID;
DELETE FROM tblGS_AVATAR WHERE txtNAME=@szCharName;
DELETE FROM tblWS_MEMO   WHERE txtNAME=@szCharName;





GO
/****** Object:  StoredProcedure [dbo].[ws_GetCharLIST]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  Stored Procedure dbo.ws_GetCharLIST    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_GetCharLIST]
	@szAccount	nvarchar(30)
AS
SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME
FROM tblGS_AVATAR
WHERE txtACCOUNT=@szAccount





GO
/****** Object:  StoredProcedure [dbo].[ws_GetFRIEND]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_GetFRIEND    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_GetFRIEND]
	@iCharIDX	int
AS
SELECT intFriendCNT, blobFRIENDS
FROM tblWS_FRIEND
WHERE intCharID=@iCharIDX





GO
/****** Object:  StoredProcedure [dbo].[ws_GetMEMO]    Script Date: 9/12/2019 2:20:43 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** Object:  Stored Procedure dbo.ws_GetMEMO    Script Date: 10/24/2005 4:10:58 PM ******/




CREATE PROCEDURE [dbo].[ws_GetMEMO]
	@szName	nvarchar(30)
AS
SELECT TOP 5
	dwDATE, txtFROM, txtMEMO
FROM tblWS_MEMO
WHERE txtNAME=@szName
ORDER BY dwDATE ASC





GO
