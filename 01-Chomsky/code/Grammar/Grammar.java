package Grammar;

import java.io.*;
import java.util.*;

public class Grammar{
    private static Set<Character> V; // ���ս���ż���
    private static Set<Character> T; // �ս���ż���
    private static List<String> P1;   // ����ʽ�󲿼���
    private static List<String> P2;   // ����ʽ�Ҳ�����
    private static char S;            // ��ʼ����

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
    // ����ķ�������������
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
        // ���� Grammar ʵ��
        Grammar grammar = new Grammar();
        // ת���ķ���ʽ
        //convertGrammarToFile("Grammar.txt", "grammar_transfered.txt");
        convertGrammarToFile("C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\Grammar.txt", "C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\grammar_transfered.txt");

        // ��ȡת������ķ�����
        readGrammarFromFile("C:\\Users\\MSG\\IdeaProjects\\NS\\src\\Grammar\\files\\grammar_transfered.txt");
        // ����ķ�����
        grammar.printGrammar();

        // �����ķ�����
        analyzeGrammarType();
    }

    // ת���ķ���ʽ
    public static void convertGrammarToFile(String inputFilePath, String outputFilePath) {
        List<String> data = new ArrayList<>();
        String s;
        int lineNum = 0;
//        Set<Character> TT = new HashSet<>();
//        // ����T��TT
//        TT.addAll(T);
        // ���ȸ���Դ�ļ���·����ȡ�ķ����ݲ��洢��data�б���
        try (BufferedReader br = new BufferedReader(new FileReader(inputFilePath))) {
            while ((s = br.readLine()) != null) {
                data.add(s);
                lineNum++;
            }
        } catch (IOException e) {
            System.err.println("Error: Unable to open file!");
            System.exit(1);
        }
        //ת���ķ���ʽ��д�����ļ�
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(outputFilePath))) {
            for (int i = 0; i < lineNum; i++) {
                String[] Grammar = new String[4]; // ���ڴ洢�ķ���������飬ÿ�������������Ĳ���
                s = data.get(i); // ��ȡ��ǰ�е��ķ�����
                int pos = s.indexOf('{'); // ���������ŵ�λ��
                int length = s.length(); // ��ȡ��ǰ�еĳ���
                String center = s.substring(pos + 1, length - 1); // ��ȡ���һ�����֮�������
                for (int j = 0; j < 4; j++) { // �����ķ�������ĸ�����
                    pos = center.indexOf('}'); // �����һ����ŵ�λ��
                    if (pos < 0) { // ���û���ҵ��һ����ţ�����ʼ���Ų���
                        // ��ʣ�ಿ����Ϊ���һ�����֣������ϻ��з�
                        Grammar[j] = center.substring(0, 1) + "\n";
                        writer.write(Grammar[j]); // ����ǰ����д���ļ�
                        //System.out.println(Grammar[j]);
                        continue; // ����������һ���ķ�����
                    }
                    // ��ȡ��ǰ���ֲ����ϻ��з�
                    Grammar[j] = center.substring(1, pos) + "\n";
                    // �������Ĳ��֣�ȥ���Ѿ�����Ĳ��ּ����������ַ�
                    center = center.substring(pos + 3);
                    writer.write(Grammar[j]); // ����ǰ����д���ļ�
                    //System.out.println(Grammar[j]);
                }
            }
        } catch (IOException e) {
            System.err.println("Error: Unable to open output file!");
            System.exit(1);
        }

    }
    // ��ȡת������ķ����ݲ������б�
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

    // �����ķ����Ͳ�������
    private static void analyzeGrammarType() {
        int grammarType = checkGrammarType();
        if (grammarType == -1) {
            System.out.println("���ķ����������ķ�");
        } else {
            System.out.println("���ķ�Ϊ " + grammarType + " ���ķ�");
        }
    }

    // ��ȡ�ķ������е�V��T��P��S
    private static void extractGrammarData(List<String> myFile) {
        String s;
        // ��ȡV����ȡ��һ�е����ݲ�ȥ�����ż���
        s = myFile.get(0);
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == ',') continue;
            V.add(s.charAt(i));
        }
        // ��ȡT����ȡ�ڶ��е����ݲ�ȥ�����ż���
        s = myFile.get(1);
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == ',') continue;
            T.add(s.charAt(i));
        }

        // ��ȡP1��P2����ȡ�����е�����
        s = myFile.get(2);
        while (!s.isEmpty()) {
            // ���Ҷ��ŵ�λ�ã��Էָ����ʽ
            int pos = s.indexOf(','); // ���Ҷ��ŵ�λ��
            String term = pos >= 0 ? s.substring(0, pos) : s; // ��ȡ����ʽ
            s = pos >= 0 ? s.substring(pos + 1) : ""; // ����s
            // ��ȡ����ʽ�󲿺��Ҳ�
            int index_of_arrow = term.indexOf("->"); // ���Ҽ�ͷ��λ��
            if (index_of_arrow != -1) { // ����ҵ��˼�ͷ
                String alpha = term.substring(0, index_of_arrow); // ��ȡ����ʽ��
                String beta = term.substring(index_of_arrow + 2);// ��ȡ����ʽ�Ҳ�
                String[] betas = beta.split("\\|"); // �����߷ָ��ѡʽ
                for (String beta1 : betas) { // ���󲿺��Ҳ��ĺ�ѡʽ��ӵ�P1��P2��
                    P1.add(alpha);
                    P2.add(beta1);
                }
            }
        }

        // ��ȡ��ʼ����
        S = myFile.get(3).charAt(0);
    }
    // �ж��ķ�����
    private static int checkGrammarType() {
        int one_of_type = 4; // ��ʼֵΪ4������δȷ������
        int type = 4; // �ķ����ͣ�Ĭ��Ϊ4����ʾδȷ������
        int is_left = 0; // �������ķ���ǣ�0��ʾ�����������ķ���1��ʾ���������ķ�
        int is_right = 0; // �������ķ���ǣ�0��ʾ�����������ķ���1��ʾ���������ķ�
        // ��������ʽ�б�
        for (int it = 0; it < P1.size(); it++) {
            if (one_of_type < type) { // �����ķ�����
                type = one_of_type;
            }
            String alpha = P1.get(it); // ��ȡ����ʽ��
            String beta = P2.get(it); // ��ȡ����ʽ�Ҳ�

            // �ж�0���ķ�������ṹ�ķ�
            // ����󲿦��Ƿ�����V��T�ļ���
            boolean validAlpha = true; // ����󲿦��Ƿ���Ч
            for (int i = 0; i < alpha.length(); i++) {
                char symbol = alpha.charAt(i);
                if (!V.contains(symbol) && !T.contains(symbol)) {
                    validAlpha = false; // �зǷ����ţ����Ϊ��Ч
                    break;
                }
            }
            //����󲿦��Ƿ����ٺ���һ�����ս��
            boolean has_Vp = false;
            for (int i = 0; i < alpha.length(); i++) {
                if (V.contains(alpha.charAt(i))) {
                    has_Vp = true; // ���з��ս��
                    break;
                }
            }
            // ����Ҳ����Ƿ�����V��T�ļ���
            boolean validBeta = true; // ����Ҳ����Ƿ���Ч
            for (int i = 0; i < beta.length(); i++) {
                char symbol = beta.charAt(i);
                if (!V.contains(symbol) && !T.contains(symbol)) {
                    if (symbol == '��'){ continue; } // �մ�
                    validBeta = false; // �зǷ����ţ����Ϊ��Ч
                    break;
                }
            }
            // ���������0���ķ�����
            if (!has_Vp||!validAlpha||!validBeta) {
                type = -1;
                break;
            }
            one_of_type = 0;

            // �ж�1���ķ����������й��ķ������Ҳ����ַ���Ҫ���ڻ�����󲿵��ַ���
            int betaLength = 0; // �Ҳ��ַ���(��ȥ�մ�)
            for (int i = 0; i < beta.length(); i++) {
                char symbol = beta.charAt(i);
                if (symbol != '��') { // ����ַ����Ǧ�
                    betaLength++;
                }
            }
            if (alpha.length() <= betaLength || betaLength == 0){
                one_of_type = 1;
            } else { continue; }

            // �ж�2���ķ����������޹��ķ�������ֻ��һ�����ս��
            if (alpha.length() == 1 && V.contains(alpha.charAt(0))) {
                one_of_type = 2;
            } else { continue; }

            // �ж�3���ķ��������ķ��������������ķ����������ķ�����
            // �Ҳ�ֻ��һ�����ս����մ�
            if (beta.length() == 1 && (T.contains(beta.charAt(0))||beta.charAt(0)=='��')) {
                one_of_type = 3;
            }
            // �Ҳ����������ţ�����һ���Ƿ��ս����һ�����ս����մ�
            else if (beta.length() == 2){
                // �������ķ�
                if((T.contains(beta.charAt(0))||beta.charAt(0)=='��') && V.contains(beta.charAt(1))) {
                    is_right = 1;
                    if(is_left == 0){ // ���������ķ����ɻ���
                        one_of_type = 3;
                    }
                }
                // �������ķ�
                else if (V.contains(beta.charAt(0)) && (T.contains(beta.charAt(1))||beta.charAt(1)=='��')) {
                    is_left = 1;
                    if(is_right == 0){ // ���������ķ����ɻ���
                        one_of_type = 3;
                    }
                }
            }
        }
        if (one_of_type < type) { // �����һ�θ����ķ�����
            type = one_of_type;
        }
        return type;
    }
}
