-- �����̸� : QuestGlobal.lua
-- ���Ͽ뵵 : 3Ddata/QUEST ��������  lua ���Ͽ��� ������������� �Լ��� ������



-- 1������
function G_giveJob_1(jobCode)

  local level
  level = GF_getVariable( 3 ) 

  if (level < 10) then    return
  end

  if     (jobCode == 111) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 211) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 311) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 411) then   GF_setVariable ( 4, jobCode )
  end
end

-- 2������


-- 3������











