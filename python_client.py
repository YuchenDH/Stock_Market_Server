import MySocket
import dicttoxml
import random
import string

def req_format(xml):
    print(xml)
    return xml

def create_account(new_account):
    xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    xml += "<create>\n"
    for (account_id, balance) in new_account:
        xml += "  <account id=\""+str(account_id)+"\" balance=\""+str(balance)+"\"/>\n"
    xml += "</create>\n"
    xml = req_format(xml)
    pass

def create_symbol(new_symbol, shares):
    xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    xml += "<create>\n"
    xml += "  <symbol sym=\"" + new_symbol + "\">\n"
    for (account_id, balance) in shares:
        xml += "    <account id=\"" + str(account_id) + "\" balance=\"" + str(balance) + "\"/>\n"
    xml += "  </symbol>\n"
    xml += "</create>\n"
    xml = req_format(xml)
    pass

def create_transaction(orders):
    xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    xml += "<transactions>\n"
    for (account, sym, amount, price) in orders:
        xml += "  <order sym=\""+sym+" amount=\""+ str(amount) + "\" limit=\"" + str(price) + "\"/>\n"
    xml += "</transactions>\n"
    xml = req_format(xml)
    pass


def main():
    pysock = MySocket.MySocket()
    pysock.connect(host="127.0.0.1", port=12345)
    sym = []
    account = []
    account_size = 1000
    symbol_size = 100
    order_size = 10000
    max_price = 100
    order_per_xml = 5
    
    for n in range(account_size):
        new_account = ''.join([random.choice('0123456789') for n in range(10)])
        account.append(new_account)
        new_balance = random.randint(1, 1000000)
        list_new_account = [(new_account, new_balance)]
        create_account(list_new_account)
        
    for n in range(symbol_size):
        new_symbol = ''.join([random.choice(string.ascii_letters + string.digits) for n in range(5)])
        sym.append(new_symbol)
        rand_int = random.randint( 0, len(account) )
        new_symbol_shares = []
        for n in range(rand_int):
            new_symbol_shares.append( (account[n], random.randint(1, 10000) ) )
        create_symbol(new_symbol, new_symbol_shares)

    for n in range(order_size):
        orders = []
        for j in range(order_per_xml):
            rand_account_index = random.randint( 0, len(account) - 1)
            rand_symbol_index = random.randint ( 0, len(sym) - 1)
            rand_shares = random.randint( 0, 1000 )
            rand_price = random.randint( -max_price, max_price )
            orders.append((account[rand_account_index], sym[rand_symbol_index], rand_shares, rand_price))
        create_transaction(orders)
    
    #xml = b"171\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<create>\n<account id=\"123456\" balance=\"1000\"/>\n<symbol sym=\"SPY\">\n  <account id=\"123456\">100000</account>\n </symbol>\n</create>\n\n"
    #print ("client msg length"+str(len(xml)))
    #pysock.send_some(xml)

if __name__ == '__main__':
    main()
