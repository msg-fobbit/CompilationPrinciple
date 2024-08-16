package Grammar;

import java.io.*;
import java.util.*;

public class Grammar{
    private static Set<Character> V; // 非终结符号集合
    private static Set<Character> T; // 终结符号集合
    private static List<String> P1;   // 产生式左部集合
    private static List<String> P2;   // 产生式右部集合
    private static char S;            // 开始符号

    public Grammar() {
        V = new HashSet<>();
        T = new HashSet<>();
        P1 = new ArrayList<>();
        P2 = new ArrayList<>();
    }

    public Set<Character> getVn() {
        return V;
    }

    public Set<Character> getVt() {
        return T;
    }

    public List<String> getP1() {
        return P1;
    }

    public List<String> getP2() {
        return P2;
    }

    public char getS() {
        return S;
    }
    // 输出文法各个部分内容
    public void printGrammar() {
        System.out.println("Non-terminal symbols (V): \n" + V);
        System.out.println("Terminal symbols (T): \n" + T);
        System.out.println("Production rules (P): ");
        for (int i = 0; i < P1.size(); i++) {
            System.out.println(P1.get(i) + "->" + P2.get(i));
        }
        System.out.println("Start symbol (S): \n" + S);
    }
    public static void main(String[] args) {
        // 创建 Grammar 实例
        Grammar grammar = new Grammar();
        // 转换文法格式
        //convertGrammarToFile("Grammar.txt", "grammar_transfered.txt");
        convertGrammarToFile("C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\Grammar.txt", "C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\grammar_transfered.txt");

        // 读取转换后的文法数据
        readGrammarFromFile("C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\grammar_transfered.txt");
        // 输出文法内容
        grammar.printGrammar();

        // 分析文法类型
        analyzeGrammarType();
    }

    // 转换文法格式
    public static void convertGrammarToFile(String inputFilePath, String outputFilePath) {
        List<String> data = new ArrayList<>();
        String s;
        int lineNum = 0;
//        Set<Character> TT = new HashSet<>();
//        // 复制T到TT
//        TT.addAll(T);
        // 首先根据源文件的路径读取文法数据并存储在data列表中
        try (BufferedReader br = new BufferedReader(new FileReader(inputFilePath))) {
            while ((s = br.readLine()) != null) {
                data.add(s);
                lineNum++;
            }
        } catch (IOException e) {
            System.err.println("Error: Unable to open file!");
            System.exit(1);
        }
        //转换文法格式并写入新文件
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(outputFilePath))) {
            for (int i = 0; i < lineNum; i++) {
                String[] Grammar = new String[4]; // 用于存储文法规则的数组，每个规则最多包含四部分
                s = data.get(i); // 获取当前行的文法规则
                int pos = s.indexOf('{'); // 查找左花括号的位置
                int length = s.length(); // 获取当前行的长度
                String center = s.substring(pos + 1, length - 1); // 截取左右花括号之间的内容
                for (int j = 0; j < 4; j++) { // 遍历文法规则的四个部分
                    pos = center.indexOf('}'); // 查找右花括号的位置
                    if (pos < 0) { // 如果没有找到右花括号，即开始符号部分
                        // 则将剩余部分作为最后一个部分，并加上换行符
                        Grammar[j] = center.substring(0, 1) + "\n";
                        writer.write(Grammar[j]); // 将当前部分写入文件
                        //System.out.println(Grammar[j]);
                        continue; // 继续处理下一个文法规则
                    }
                    // 截取当前部分并加上换行符
                    Grammar[j] = center.substring(1, pos) + "\n";
                    // 更新中心部分，去掉已经处理的部分及其后的两个字符
                    center = center.substring(pos + 3);
                    writer.write(Grammar[j]); // 将当前部分写入文件
                    //System.out.println(Grammar[j]);
                }
            }
        } catch (IOException e) {
            System.err.println("Error: Unable to open output file!");
            System.exit(1);
        }

    }
    // 读取转换后的文法数据并返回列表
    private static void readGrammarFromFile(String filePath) {
        List<String> myFile = new ArrayList<>();
        try (BufferedReader infile = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = infile.readLine()) != null) {
                myFile.add(line);
            }
        } catch (IOException e) {
            System.err.println("Error: Unable to open file!");
            e.printStackTrace();
            return;
        }
        extractGrammarData(myFile);
    }

    // 分析文法类型并输出结果
    private static void analyzeGrammarType() {
        int grammarType = checkGrammarType();
        if (grammarType == -1) {
            System.out.println("该文法不是正规文法");
        } else {
            System.out.println("该文法为 " + grammarType + " 型文法");
        }
    }

    // 提取文法数据中的V、T、P、S
    private static void extractGrammarData(List<String> myFile) {
        String s;
        // 提取V，获取第一行的内容并去掉逗号即可
        s = myFile.get(0);
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == ',') continue;
            V.add(s.charAt(i));
        }
        // 提取T，获取第二行的内容并去掉逗号即可
        s = myFile.get(1);
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == ',') continue;
            T.add(s.charAt(i));
        }

        // 提取P1和P2，获取第三行的内容
        s = myFile.get(2);
        while (!s.isEmpty()) {
            // 查找逗号的位置，以分割产生式
            int pos = s.indexOf(','); // 查找逗号的位置
            String term = pos >= 0 ? s.substring(0, pos) : s; // 截取产生式
            s = pos >= 0 ? s.substring(pos + 1) : ""; // 更新s
            // 提取产生式左部和右部
            int index_of_arrow = term.indexOf("->"); // 查找箭头的位置
            if (index_of_arrow != -1) { // 如果找到了箭头
                String alpha = term.substring(0, index_of_arrow); // 提取产生式左部
                String beta = term.substring(index_of_arrow + 2);// 提取产生式右部
                String[] betas = beta.split("\\|"); // 以竖线分割候选式
                for (String beta1 : betas) { // 将左部和右部的候选式添加到P1和P2中
                    P1.add(alpha);
                    P2.add(beta1);
                }
            }
        }

        // 提取开始符号
        S = myFile.get(3).charAt(0);
    }
    // 判断文法类型
    private static int checkGrammarType() {
        int one_of_type = 4; // 初始值为4，代表未确定类型
        int type = 4; // 文法类型，默认为4，表示未确定类型
        int is_left = 0; // 左线性文法标记，0表示不是左线性文法，1表示是左线性文法
        int is_right = 0; // 右线性文法标记，0表示不是右线性文法，1表示是右线性文法
        // 遍历产生式列表
        for (int it = 0; it < P1.size(); it++) {
            if (one_of_type < type) { // 更新文法类型
                type = one_of_type;
            }
            String alpha = P1.get(it); // 获取产生式左部
            String beta = P2.get(it); // 获取产生式右部

            // 判断0型文法，短语结构文法
            // 检查左部α是否属于V∪T的集合
            boolean validAlpha = true; // 标记左部α是否有效
            for (int i = 0; i < alpha.length(); i++) {
                char symbol = alpha.charAt(i);
                if (!V.contains(symbol) && !T.contains(symbol)) {
                    validAlpha = false; // 有非法符号，标记为无效
                    break;
                }
            }
            //检查左部α是否至少含有一个非终结符
            boolean has_Vp = false;
            for (int i = 0; i < alpha.length(); i++) {
                if (V.contains(alpha.charAt(i))) {
                    has_Vp = true; // 含有非终结符
                    break;
                }
            }
            // 检查右部β是否属于V∪T的集合
            boolean validBeta = true; // 标记右部β是否有效
            for (int i = 0; i < beta.length(); i++) {
                char symbol = beta.charAt(i);
                if (!V.contains(symbol) && !T.contains(symbol)) {
                    if (symbol == 'ε'){ continue; } // 空串
                    validBeta = false; // 有非法符号，标记为无效
                    break;
                }
            }
            // 如果不符合0型文法定义
            if (!has_Vp||!validAlpha||!validBeta) {
                type = -1;
                break;
            }
            one_of_type = 0;

            // 判断1型文法，上下文有关文法，即右部的字符数要大于或等于左部的字符数
            int betaLength = 0; // 右部字符数(除去空串)
            for (int i = 0; i < beta.length(); i++) {
                char symbol = beta.charAt(i);
                if (symbol != 'ε') { // 如果字符不是ε
                    betaLength++;
                }
            }
            if (alpha.length() <= betaLength || betaLength == 0){
                one_of_type = 1;
            } else { continue; }

            // 判断2型文法，上下文无关文法，即左部只有一个非终结符
            if (alpha.length() == 1 && V.contains(alpha.charAt(0))) {
                one_of_type = 2;
            } else { continue; }

            // 判断3型文法，正规文法，符合左线性文法或右线性文法即可
            // 右部只有一个非终结符或空串
            if (beta.length() == 1 && (T.contains(beta.charAt(0))||beta.charAt(0)=='ε')) {
                one_of_type = 3;
            }
            // 右部有两个符号，并且一个是非终结符，一个是终结符或空串
            else if (beta.length() == 2){
                // 右线性文法
                if((T.contains(beta.charAt(0))||beta.charAt(0)=='ε') && V.contains(beta.charAt(1))) {
                    is_right = 1;
                    if(is_left == 0){ // 左、右线性文法不可混用
                        one_of_type = 3;
                    }
                }
                // 左线性文法
                else if (V.contains(beta.charAt(0)) && (T.contains(beta.charAt(1))||beta.charAt(1)=='ε')) {
                    is_left = 1;
                    if(is_right == 0){ // 左、右线性文法不可混用
                        one_of_type = 3;
                    }
                }
            }
        }
        if (one_of_type < type) { // 最后再一次更新文法类型
            type = one_of_type;
        }
        return type;
    }
}
